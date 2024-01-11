// DlgTabSocket.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgDiagServer.h"
#include "DlgTabSocket.h"

#include "..\..\INCLUDE\util.h"

// CDlgTabSocket dialog

// -------------------------------------------------------------------------------------------------------------------------------------------------

COLUMN SocketsColumn[] = 
{
	{	_T("Адрес"),				150,	LVCFMT_LEFT,		CDlgTabSocket::DrawIP						},
	{	_T("Передано"),			90,		LVCFMT_CENTER,	CDlgTabSocket::DrawSentBytes		},
	{	_T("Онисание"),			150,	LVCFMT_LEFT,		CDlgTabSocket::DrawDescription	},
	{	_T("Время работы"),	120,	LVCFMT_CENTER,	CDlgTabSocket::DrawThreadTime		},
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgTabSocket, CDialog)

CDlgTabSocket::CDlgTabSocket(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTabSocket::IDD, pParent)
{

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgTabSocket::~CDlgTabSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOCKETS_LIST, m_List);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgTabSocket, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO,	IDC_SOCKETS_LIST,	&CDlgTabSocket::OnLvnGetdispinfoSocketsList)
	ON_NOTIFY(NM_DBLCLK,				IDC_SOCKETS_LIST,	&CDlgTabSocket::OnNMDblclkSocketsList)
	ON_NOTIFY(NM_RCLICK,				IDC_SOCKETS_LIST, &CDlgTabSocket::OnNMRClickSocketsList)
	ON_NOTIFY(LVN_ITEMCHANGED,	IDC_SOCKETS_LIST,	&CDlgTabSocket::OnSelectSocket)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTabSocket message handlers
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::DrawIP( int item, int subItem, LPVOID param, CString& str )
{
	CSocketThread* pSocket = theApp.ClientSocketBase.GetAt(item);
	if (!pSocket)
		return;

	sockaddr_in sa;
	sa.sin_addr.s_addr	= pSocket->GetIP();

	str.Format(_T("%d.%d.%d.%d : %d"),	sa.sin_addr.S_un.S_un_b.s_b1,
																			sa.sin_addr.S_un.S_un_b.s_b2,
																			sa.sin_addr.S_un.S_un_b.s_b3,
																			sa.sin_addr.S_un.S_un_b.s_b4,
																			theApp.ServerPort);
}

void CDlgTabSocket::DrawSentBytes( int item, int subItem, LPVOID param, CString& str )
{
	CSocketThread* pSocket = theApp.ClientSocketBase.GetAt(item);
	if (!pSocket)
		return;

	str = pSocket->GetSentBytesStr();
}

void CDlgTabSocket::DrawDescription( int item, int subItem, LPVOID param, CString& str )
{
	CSocketThread* pSocket = theApp.ClientSocketBase.GetAt(item);
	if (!pSocket)
		return;

	str = pSocket->Description;
}

void CDlgTabSocket::DrawThreadTime( int item, int subItem, LPVOID param, CString& str )
{
	CSocketThread* pSocket = theApp.ClientSocketBase.GetAt(item);
	if (!pSocket)
		return;

	str = pSocket->GetTimeLivingStr();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabSocket::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	InitListCtrlColumns( m_List, SocketsColumn, sizeof(SocketsColumn) );

	m_List.SetItemCount(theApp.ClientSocketBase.GetCount());

	m_List.EnsureVisible( 0, TRUE );
	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	TimerID = (UINT) SetTimer( 1, 250, NULL );

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::OnLvnGetdispinfoSocketsList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	int nItem = pDispInfo->item.iItem;
	int nColumn = pDispInfo->item.iSubItem;

	if (nItem == -1 || nColumn == -1)
		return;

	//if (pDispInfo->item.mask & LVIF_INDENT)
	//{
	//	if (theApp.ClientSocketBase.GetAt(nItem)->pCommThread)
	//	{
	//		if (!theApp.ClientSocketBase.GetAt(nItem)->pCommThread->ReceivedBytesOk)
	//			pDispInfo->item.iIndent = RGB(0xFF, 0xFF, 0xCC);

	//		if (theApp.ClientSocketBase.GetAt(nItem)->pCommThread->nCommState != COMM_PORT_STATE_OK)
	//			pDispInfo->item.iIndent = RGB(0xFA, 0xD0, 0xD0);
	//	}
	//}

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;
		SocketsColumn[nColumn].pDrawProc( nItem, nColumn, this, str );
		lstrcpyn(pDispInfo->item.pszText, str.GetBuffer(), str.GetLength() + 1);
	}

	
}

// --------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::OnSelectSocket(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	//if (!(pNMLV->uChanged & LVIF_STATE) || pNMLV->uOldState)
	//	return;

	//int nSelectSocket = pNMLV->iItem;
	//if (nSelectSocket == -1)
	//{
	//	theApp.ClientSocketBase.SetActiveSocket(NULL);
	//	return;
	//}

	//CSocketThread* pSocket = theApp.ClientSocketBase.GetAt(nSelectSocket);
	//theApp.ClientSocketBase.SetActiveSocket(pSocket);

	//CDlgDiagServer* pParent = (CDlgDiagServer*) GetParent();
	//if (!pParent)
	//	return;

	//pParent->UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::OnNMDblclkSocketsList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	//int nItem = pNMItemActivate->iItem;
	//if (nItem == -1)
		//CreateSocket();
	//else
		//EditSocket();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::OnNMRClickSocketsList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

 //  CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	// if (!pMainMenu)
	//	 return;

 //  CMenu* pSubMenu = pMainMenu->GetSubMenu(0);    
	// if (!pSubMenu)
	//	 return;
 //  
 //  ClientToScreen(&pNMItemActivate->ptAction);
 //  pSubMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, AfxGetMainWnd());
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_List.m_hWnd)
		m_List.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSocket::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerID)
	{
		if (theApp.ClientSocketBase.GetCount() != m_List.GetItemCount() )
			m_List.SetItemCount(theApp.ClientSocketBase.GetCount());

		m_List.RedrawItems(0, m_List.GetItemCount());
	}

	CDialog::OnTimer(nIDEvent);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

