// DlgTabMemory.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgTabMemory.h"

#include "DataFile.h"

// CDlgTabMemory dialog

// -------------------------------------------------------------------------------------------------------------------------------------------------

COLUMN MemoryColumn[] = 
{
	{	_T("N"),						30,		LVCFMT_LEFT,		CDlgTabMemory::DrawParamNo			},
	{	_T("Тип значение"),	1,		LVCFMT_CENTER,	CDlgTabMemory::DrawTypeValue		},
	{	_T("Значение"),			90,		LVCFMT_CENTER,	CDlgTabMemory::DrawValue				},
	{	_T("Тип"),					100,	LVCFMT_CENTER,	CDlgTabMemory::DrawType					},
	{	_T("Онисание"),			300,	LVCFMT_LEFT,		CDlgTabMemory::DrawDescription	},
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgTabMemory, CDialog)

CDlgTabMemory::CDlgTabMemory(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTabMemory::IDD, pParent)
{
	LoadMemoryParamArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgTabMemory::~CDlgTabMemory()
{
	SaveMemoryParamArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EEPROM_LIST, m_List);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgTabMemory, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO,	IDC_EEPROM_LIST,	&CDlgTabMemory::OnLvnGetdispinfoMemoryList)
	ON_NOTIFY(NM_DBLCLK,				IDC_EEPROM_LIST,	&CDlgTabMemory::OnNMDblclkMemoryList)
	ON_NOTIFY(NM_RCLICK,				IDC_EEPROM_LIST,	&CDlgTabMemory::OnNMRClickMemoryList)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTabMemory message handlers
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::DrawParamNo( int item, int subItem, LPVOID param, CString& str )
{
	str.Format(_T("%d"), item + 1);
}

void CDlgTabMemory::DrawTypeValue( int item, int subItem, LPVOID param, CString& str )
{	
	CDlgTabMemory* pThis = (CDlgTabMemory*) param;
	if (!pThis)
		return;

	EEPROM_PARAM MemoryParam = pThis->MemoryParamArray[item];
	if (MemoryParam.ValueType < 0 || MemoryParam.ValueType >= EEPROM_PARAM_VALUE_TYPE_COUNT)
	{
		str = _T("Не определен");
		return;
	}

	str = MemoryParamValueTypeStr[MemoryParam.ValueType];
}

void CDlgTabMemory::DrawValue( int item, int subItem, LPVOID param, CString& str )
{	
	CDlgTabMemory* pThis = (CDlgTabMemory*) param;
	if (!pThis)
		return;

	EEPROM_PARAM MemoryParam = pThis->MemoryParamArray[item];

	str.Format(_T("%3d"), MemoryParam.Value); 
}

void CDlgTabMemory::DrawType( int item, int subItem, LPVOID param, CString& str )
{	
	CDlgTabMemory* pThis = (CDlgTabMemory*) param;
	if (!pThis)
		return;
	EEPROM_PARAM MemoryParam = pThis->MemoryParamArray[item];
	if (MemoryParam.Type < 0 || MemoryParam.Type >= EEPROM_PARAM_TYPE_COUNT)
	{
		str = _T("Не определен");
		return;
	}

	str = MemoryParamTypeStr[MemoryParam.Type];
}

void CDlgTabMemory::DrawDescription( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabMemory* pThis = (CDlgTabMemory*) param;
	if (!pThis)
		return;
	EEPROM_PARAM MemoryParam = pThis->MemoryParamArray[item];

	str = MemoryParam.Description;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabMemory::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	InitListCtrlColumns( m_List, MemoryColumn, sizeof(MemoryColumn) );

	m_List.SetItemCount((int) MemoryParamArray.GetCount());

	m_List.EnsureVisible( 0, TRUE );
	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	ZeroMemory(CurrentMemoryByteState, EEPROM_BYTE_COUNT * sizeof(BYTE));
	TimerID = (UINT) SetTimer( 1, 250, NULL );

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::OnLvnGetdispinfoMemoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	int nItem = pDispInfo->item.iItem;
	int nColumn = pDispInfo->item.iSubItem;

	if (nItem == -1 || nColumn == -1)
		return;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;
		MemoryColumn[nColumn].pDrawProc( nItem, nColumn, this, str );
		lstrcpyn(pDispInfo->item.pszText, str.GetBuffer(), str.GetLength() + 1);
	}

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::LoadMemoryParamArray()
{
	MemoryParamArray.SetSize(EEPROM_BYTE_COUNT);

	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_EEPROM_PARAM))
		return;

	int nParamCount = (int) MemoryParamArray.GetCount();
	for (int nParam = 0; nParam < nParamCount; nParam++)
	{
		//MemoryParamArray[nParam].ValueType = MemoryParamValueType[nParam];

		Table.ReadRecord(nParam, &MemoryParamArray[nParam]);
	}

	Table.Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::SaveMemoryParamArray()
{
	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_EEPROM_PARAM))
		return;

	Table.Empty();
	
	int nRecordCount = (int) MemoryParamArray.GetCount();
	for(int p = 0; p < nRecordCount ; p++)
		Table.WriteRecord(&MemoryParamArray[p]);
	
	Table.Close();
}
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::UpdateMemoryParamArray()
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;
	
	for(int nByte = 0; nByte < EEPROM_BYTE_COUNT; nByte++)
	{
		MemoryParamArray[nByte].Value = pActiveSource->MemoryByteState[nByte];

		if (CurrentMemoryByteState[nByte] != MemoryParamArray[nByte].Value)
		{
			CurrentMemoryByteState[nByte] = MemoryParamArray[nByte].Value;
			m_List.RedrawItems(nByte, nByte);
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::ChangeMemoryParamState()
{
	int nItem = 	GetSelectedItemIndex(m_List);
	if (nItem == -1)
	{
		AfxMessageBox(_T("Не выбран параметр для редактирования!"));
		return;
	}

	EEPROM_PARAM MemoryParam = MemoryParamArray[nItem];

	CDlgMemoryParamState DlgMemoryParamState(MemoryParam.Value);
	if(DlgMemoryParamState.DoModal() != IDOK)
		return;

	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource || !pActiveSource->pCommThread)
		return;

	pActiveSource->pCommThread->RequestSetMemoryParam(nItem, DlgMemoryParamState.m_Byte);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::EditMemoryParam()
{
	int nItem = GetSelectedItemIndex(m_List);
	if (nItem == -1)
	{
		AfxMessageBox(_T("Не выбран вход для редактирования!"));
		return;
	}

	EEPROM_PARAM MemoryParam = MemoryParamArray[nItem];

	CDlgMemoryParam DlgMemoryParam(MemoryParam);
	if(DlgMemoryParam.DoModal() != IDOK)
		return;

	MemoryParamArray[nItem] = DlgMemoryParam.m_MemoryParam; 

	m_List.RedrawItems(nItem, nItem);

	SaveMemoryParamArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::OnNMDblclkMemoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nItem = pNMItemActivate->iItem;
	if (nItem == -1)
		return;

	ChangeMemoryParamState();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::OnNMRClickMemoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

   CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	 if (!pMainMenu)
		 return;

   CMenu* pSubMenu = pMainMenu->GetSubMenu(3);    
	 if (!pSubMenu)
		 return;
   
   ClientToScreen(&pNMItemActivate->ptAction);
   pSubMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, AfxGetMainWnd());
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_List.m_hWnd)
		m_List.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabMemory::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerID)
		UpdateMemoryParamArray();

	CDialog::OnTimer(nIDEvent);
}

// CDlgMemoryParam dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgMemoryParam, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMemoryParam::CDlgMemoryParam(EEPROM_PARAM MemoryParam, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMemoryParam::IDD, pParent)
	, m_MemoryParam(MemoryParam)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMemoryParam::~CDlgMemoryParam()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMemoryParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_EEPROM_PARAM_DESCRIPTION,	str);
		SafeStrncpy(m_MemoryParam.Description, str, EEPROM_PARAM_DESCRIPTION_LEN );	
	}
	else
		DDX_Text(pDX, IDC_EEPROM_PARAM_DESCRIPTION,	(CString)m_MemoryParam.Description);

	DDX_Control(pDX, IDC_EEPROM_PARAM_TYPE, m_MemoryParamType);
	if (pDX->m_bSaveAndValidate)
		m_MemoryParam.Type = m_MemoryParamType.GetCurSel();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgMemoryParam, CDialog)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgMemoryParam::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int p = 0; p < EEPROM_PARAM_TYPE_COUNT; p++)
		m_MemoryParamType.AddString(MemoryParamTypeStr[p]);

	int nMemoryParamType = m_MemoryParam.Type;
	if (nMemoryParamType == EEPROM_PARAM_TYPE_UNKNOWN)
		nMemoryParamType = 0;

	m_MemoryParamType.SetCurSel(nMemoryParamType);

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

// CDlgMemoryParamState dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgMemoryParamState, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMemoryParamState::CDlgMemoryParamState(BYTE Byte, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMemoryParamState::IDD, pParent)
	, m_Byte(Byte)
{

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgMemoryParamState::~CDlgMemoryParamState()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMemoryParamState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EEPROM_BYTE_DEC, m_ByteDec);
	DDX_Control(pDX, IDC_EEPROM_BYTE_HEX, m_ByteHex);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgMemoryParamState, CDialog)
	ON_EN_CHANGE(IDC_EEPROM_BYTE_DEC, &CDlgMemoryParamState::OnChangeByteDec)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgMemoryParamState::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	str.Format(_T("%d"), m_Byte);
	m_ByteDec.SetWindowText(str);

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgMemoryParamState::OnChangeByteDec()
{
	CString str;

	m_ByteDec.GetWindowText(str);
	BYTE nByte = StrToInt(str);
	str.Format(_T("%02X"), nByte);
	m_ByteHex.SetWindowText(str);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------


void CDlgMemoryParamState::OnOK()
{
	CString str;

	m_ByteDec.GetWindowText(str);
	m_Byte = StrToInt(str);

	CDialog::OnOK();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
