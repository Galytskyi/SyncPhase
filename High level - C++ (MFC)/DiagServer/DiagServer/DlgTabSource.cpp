// DlgTabSource.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgDiagServer.h"
#include "DlgTabSource.h"

#include "..\..\INCLUDE\util.h"
#include "..\..\INCLUDE\GetComm.h"

// CDlgTabSource dialog

// -------------------------------------------------------------------------------------------------------------------------------------------------

COLUMN SourcesColumn[] = 
{
	{	_T("COM-порт"),			70,		LVCFMT_LEFT,		CDlgTabSource::DrawComm					},
	{	_T("Онисание"),			150,	LVCFMT_LEFT,		CDlgTabSource::DrawDescription	},
	{	_T("Соеденение"),		100,	LVCFMT_CENTER,	CDlgTabSource::DrawConnect			},
	{	_T("Принято"),			90,		LVCFMT_CENTER,	CDlgTabSource::DrawReceived			},
	{	_T("Время работы"),	120,	LVCFMT_CENTER,	CDlgTabSource::DrawThreadTime		},
	{	_T("Ошибок КС"),		90,		LVCFMT_CENTER,	CDlgTabSource::DrawErrorCRC			},
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgTabSource, CDialog)

CDlgTabSource::CDlgTabSource(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTabSource::IDD, pParent)
{

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgTabSource::~CDlgTabSource()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCES_LIST, m_List);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgTabSource, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO,	IDC_SOURCES_LIST,	&CDlgTabSource::OnLvnGetdispinfoSourcesList)
	ON_NOTIFY(NM_DBLCLK,				IDC_SOURCES_LIST,	&CDlgTabSource::OnNMDblclkSourcesList)
	ON_NOTIFY(NM_RCLICK,				IDC_SOURCES_LIST, &CDlgTabSource::OnNMRClickSourcesList)
	ON_NOTIFY(LVN_ITEMCHANGED,	IDC_SOURCES_LIST,	&CDlgTabSource::OnSelectSource)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTabSource message handlers
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::DrawComm( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pSource = theApp.SourceBase.GetAt(item);
	if (!pSource)
		return;

	str = pSource->Comm;
}

void CDlgTabSource::DrawDescription( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pSource = theApp.SourceBase.GetAt(item);
	if (!pSource)
		return;

	str = pSource->Description;
}

void CDlgTabSource::DrawConnect( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pSource = theApp.SourceBase.GetAt(item);
	if (!pSource)
		return;

	str = pSource->GetState(); 
}

void CDlgTabSource::DrawReceived( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pSource = theApp.SourceBase.GetAt(item);
	if (!pSource || !pSource->pCommThread)
		return;

	str = pSource->pCommThread->CommState.GetReceivedBytesStr();
}

void CDlgTabSource::DrawThreadTime( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pSource = theApp.SourceBase.GetAt(item);
	if (!pSource || !pSource->pCommThread)
		return;

	str = pSource->pCommThread->CommState.GetTimeLivingStr();
}

void CDlgTabSource::DrawErrorCRC( int item, int subItem, LPVOID param, CString& str )
{
	CSourceItem* pSource = theApp.SourceBase.GetAt(item);
	if (!pSource || !pSource->pCommThread)
		return;

	str.Format(_T("%d"), pSource->pCommThread->CommState.ErrorCRC); 
}
// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabSource::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	InitListCtrlColumns( m_List, SourcesColumn, sizeof(SourcesColumn) );

	m_List.SetItemCount(theApp.SourceBase.GetCount());

	m_List.EnsureVisible( 0, TRUE );
	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	TimerID = (UINT) SetTimer( 1, 250, NULL );

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::OnLvnGetdispinfoSourcesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	int nItem = pDispInfo->item.iItem;
	int nColumn = pDispInfo->item.iSubItem;

	if (nItem == -1 || nColumn == -1)
		return;

	if (pDispInfo->item.mask & LVIF_INDENT)
	{
		if (theApp.SourceBase.GetAt(nItem)->pCommThread)
		{
			if (!theApp.SourceBase.GetAt(nItem)->pCommThread->CommState.ReceivedBytesOk)
				pDispInfo->item.iIndent = RGB(0xFF, 0xFF, 0xCC);

			if (theApp.SourceBase.GetAt(nItem)->pCommThread->CommState.nCommState != COMM_PORT_STATE_OK)
				pDispInfo->item.iIndent = RGB(0xFA, 0xD0, 0xD0);
		}
	}

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;
		SourcesColumn[nColumn].pDrawProc( nItem, nColumn, this, str );
		lstrcpyn(pDispInfo->item.pszText, str.GetBuffer(), str.GetLength() + 1);
	}

	
}

// --------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::OnSelectSource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (!(pNMLV->uChanged & LVIF_STATE) || pNMLV->uOldState)
		return;

	int nSelectSource = pNMLV->iItem;
	if (nSelectSource == -1)
	{
		theApp.SourceBase.SetActiveSource(NULL);
		return;
	}

	CSourceItem* pSource = theApp.SourceBase.GetAt(nSelectSource);
	theApp.SourceBase.SetActiveSource(pSource);

	CDlgDiagServer* pParent = (CDlgDiagServer*) GetParent();
	if (!pParent)
		return;

	pParent->UpadateMenu();
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabSource::CreateSource()
{
	CDlgSource DlgSource(NULL);
	if(DlgSource.DoModal() != IDOK)
	{
		if (DlgSource.m_pSource)
			delete DlgSource.m_pSource;
		return FALSE;
	}

	int nSelectSource = theApp.SourceBase.Add(DlgSource.m_pSource);

	m_List.SetItemCount(theApp.SourceBase.GetCount());

	m_List.EnsureVisible( nSelectSource, TRUE );
	m_List.SetItemState( nSelectSource, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	theApp.SourceBase.Save();
	
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabSource::EditSource()
{
	int nSelectSource = GetSelectedItemIndex(m_List);
	if (nSelectSource == -1)
	{
		AfxMessageBox(_T("Не выбран элемент для редактирования!"));
		return FALSE;
	}

	CSourceItem* pSource = theApp.SourceBase.GetAt(nSelectSource);
	if (!pSource)
		return FALSE;

	CDlgSource DlgSource(pSource);
	if(DlgSource.DoModal() != IDOK)
		return FALSE;

	if (!pSource->pCommThread)
		return FALSE;

	pSource->pCommThread->ReConnect();

	theApp.SourceBase.Save();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabSource::DeleteSource()
{
	int nSelectSource = GetSelectedItemIndex(m_List);
	if (nSelectSource == -1)
	{
		AfxMessageBox(_T("Не выбран элемент для удаления!"));
		return FALSE;
	}

	theApp.SourceBase.Remove(nSelectSource);

	m_List.SetItemCount(theApp.SourceBase.GetCount());

	nSelectSource--;
	if (nSelectSource == -1)
		nSelectSource = 0;

	CSourceItem* pSource = theApp.SourceBase.GetAt(nSelectSource);
	theApp.SourceBase.SetActiveSource(pSource);

	m_List.EnsureVisible( nSelectSource, TRUE );
	m_List.SetItemState( nSelectSource, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	theApp.SourceBase.Save();

	return TRUE;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::UpdateStatusBar()
{
	CDlgDiagServer* pParent = (CDlgDiagServer*) GetParent();
	if (!pParent)
		return;

	pParent->UpdateStatusBar();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::OnNMDblclkSourcesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nItem = pNMItemActivate->iItem;
	if (nItem == -1)
		CreateSource();
	else
		EditSource();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::OnNMRClickSourcesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

   CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	 if (!pMainMenu)
		 return;

   CMenu* pSubMenu = pMainMenu->GetSubMenu(0);    
	 if (!pSubMenu)
		 return;
   
   ClientToScreen(&pNMItemActivate->ptAction);
   pSubMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, AfxGetMainWnd());
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_List.m_hWnd)
		m_List.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabSource::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerID)
	{
		m_List.RedrawItems(0, m_List.GetItemCount());
		UpdateStatusBar();
	}

	CDialog::OnTimer(nIDEvent);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgSource, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgSource::CDlgSource(CSourceItem* pSource, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSource::IDD, pParent)
	, m_pSource(pSource)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgSource::~CDlgSource()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgSource::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCE_COM,					m_Comm);
	DDX_Control(pDX, IDC_SOURCE_DESCRIPTION,	m_Description);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgSource, CDialog)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgSource::OnInitDialog()
{
	CDialog::OnInitDialog();

	CStringList ComList;

	EnumComm(ComList);
	for(int i = 0; i < ComList.GetCount(); i++)
		m_Comm.AddString(ComList.GetAt(ComList.FindIndex(i)));
	

	if (!m_pSource)
	{
		SetWindowText(_T("Создание нового источника"));
		m_Comm.SetCurSel(0);
		m_Description.SetWindowText(_T("Новый источник"));
	}
	else
		SetWindowText(_T("Редактирование источника"));

	SetDlgData();

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgSource::GetDlgData()
{
	CString strComm;
	CString strDescription;

	m_Comm.GetWindowText(strComm);
	m_Description.GetWindowText(strDescription);

	if (strComm.IsEmpty())
	{
		AfxMessageBox(_T("Наимеование COM-порта не может оставаться пустым.\nВыберите, пожалуйста, COM-порт."));
		m_Comm.SetFocus();
		return FALSE;

	}

	if(!theApp.SourceBase.CheckComm(strComm) && !m_pSource)
	{
		AfxMessageBox(_T("Выбранный Вами COM-порт уже занят.\nПожалуйста, выберите другой COM-порт."));
		return FALSE;
	}

	if (!m_pSource)
		m_pSource = new CSourceItem(theApp.SourceBase.GetCount(), strComm, strDescription);
	else
	{
		SafeStrncpy( m_pSource->Comm, strComm, SOURCE_COMM_LEN );	
		SafeStrncpy( m_pSource->Description, strDescription, SOURCE_DESCRIPTION_LEN );	
	}

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgSource::SetDlgData()
{
	if (!m_pSource)
		return;

	m_Comm.SetWindowText(m_pSource->Comm);
	m_Description.SetWindowText(m_pSource->Description);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgSource::OnOK()
{
	if (!GetDlgData())
		return;

	CDialog::OnOK();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
