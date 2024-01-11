// DlgTabMessage.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgTabMessage.h"

#include "..\..\INCLUDE\util.h"

// CDlgTabMessage dialog

// -------------------------------------------------------------------------------------------------------------------------------------------------

COLUMN MessagesColumn[] = 
{
	{	_T("N"),						30,		LVCFMT_LEFT,		CDlgTabMessage::DrawNo						},
	{	_T("Время"),				100,	LVCFMT_CENTER,	CDlgTabMessage::DrawTime					},
	{	_T("Дата"),					100,	LVCFMT_CENTER,	CDlgTabMessage::DrawDate					},
	{	_T("Сообщение"),		400,	LVCFMT_LEFT,		CDlgTabMessage::DrawMessage				},
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgTabMessage, CDialog)

CDlgTabMessage::CDlgTabMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTabMessage::IDD, pParent)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgTabMessage::~CDlgTabMessage()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGES_LIST, m_List);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgTabMessage, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO,	IDC_MESSAGES_LIST,	&CDlgTabMessage::OnLvnGetdispinfoMessagesList)
	ON_NOTIFY(NM_RCLICK,				IDC_MESSAGES_LIST,	&CDlgTabMessage::OnNMRClickMessagesList)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTabMessage message handlers
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMessage::DrawNo( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;
	
	DIAG_MSG_STR Msg = pActiveSource->MsgStrArray.GetAt(item);

	str.Format(	_T("%d"),	item + 1);
}

void CDlgTabMessage::DrawTime( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;
	
	DIAG_MSG_STR Msg = pActiveSource->MsgStrArray.GetAt(item);

	str = Msg.TimeStr;
}
	

void CDlgTabMessage::DrawDate( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;
	
	DIAG_MSG_STR Msg = pActiveSource->MsgStrArray.GetAt(item);

	str = Msg.DateStr;
}

void CDlgTabMessage::DrawMessage( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;

	DIAG_MSG_STR Msg = pActiveSource->MsgStrArray.GetAt(item);

	str = Msg.MsgStr;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabMessage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	InitListCtrlColumns( m_List, MessagesColumn, sizeof(MessagesColumn) );

	TimerID = (UINT) SetTimer( 1, 250, NULL );

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMessage::OnLvnGetdispinfoMessagesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	int nItem = pDispInfo->item.iItem;
	int nColumn = pDispInfo->item.iSubItem;

	if (nItem == -1 || nColumn == -1)
		return;

	if (pDispInfo->item.mask & LVIF_INDENT)
	{
		switch(theApp.SourceBase.GetActiveSource()->MsgStrArray.GetAt(nItem).StrType)
		{
			case COMM_MSG_TYPE_STR_WARNING:	pDispInfo->item.iIndent = RGB(0xFF, 0xFF, 0xCC);	break;
			case COMM_MSG_TYPE_STR_ERROR:		pDispInfo->item.iIndent = RGB(0xFA, 0xD0, 0xD0);	break;
		}
	}

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;
		MessagesColumn[nColumn].pDrawProc( nItem, nColumn, this, str );
		lstrcpyn(pDispInfo->item.pszText, str.GetBuffer(), str.GetLength() + 1);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMessage::OnNMRClickMessagesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

   CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	 if (!pMainMenu)
		 return;

   CMenu* pSubMenu = pMainMenu->GetSubMenu(1);    
	 if (!pSubMenu)
		 return;
   
   ClientToScreen(&pNMItemActivate->ptAction);
   pSubMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, AfxGetMainWnd());

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMessage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_List.m_hWnd)
		m_List.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMessage::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerID)
	{
		CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
		if (pActiveSource)
		{
			if (m_List.GetItemCount() != pActiveSource->MsgStrArray.GetCount())
				m_List.SetItemCount(pActiveSource->MsgStrArray.GetCount());

			if (pActiveSource->EnableRecvMsg && pActiveSource->AutoEnsureMsg)
				m_List.EnsureVisible(m_List.GetItemCount() - 1, TRUE);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------