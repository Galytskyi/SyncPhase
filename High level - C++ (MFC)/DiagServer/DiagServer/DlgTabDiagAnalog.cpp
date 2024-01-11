// DlgTabDiagAnalog.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgTabDiagAnalog.h"

#include "DataFile.h"

// CDlgTabDiagAnalog dialog

// -------------------------------------------------------------------------------------------------------------------------------------------------

COLUMN DiagAnalogColumn[] = 
{
	{	_T("N"),							30,		LVCFMT_CENTER,	CDlgTabDiagAnalog::DrawNo							},
	{	_T("Ножка"),					50,		LVCFMT_CENTER,	CDlgTabDiagAnalog::DrawContact				},
	{	_T("АЦП (Dec)"),			70,		LVCFMT_CENTER,	CDlgTabDiagAnalog::DrawStateDec				},
	{	_T("АЦП (Hex)"),			70,		LVCFMT_CENTER,	CDlgTabDiagAnalog::DrawStateHex				},
	{	_T("Перобразование"),	100,	LVCFMT_CENTER,	CDlgTabDiagAnalog::DrawStateCoversion	},
	{	_T("Диапазон"),				90,		LVCFMT_CENTER,	CDlgTabDiagAnalog::DrawLimit					},
	{	_T("Описание"),				250,	LVCFMT_LEFT,		CDlgTabDiagAnalog::DrawDescription		},
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgTabDiagAnalog, CDialog)

CDlgTabDiagAnalog::CDlgTabDiagAnalog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTabDiagAnalog::IDD, pParent)
{
	LoadAnalogInputArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgTabDiagAnalog::~CDlgTabDiagAnalog()
{
	SaveAnalogInputArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DIAG_ANALOG_LIST, m_List);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgTabDiagAnalog, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO,	IDC_DIAG_ANALOG_LIST,	&CDlgTabDiagAnalog::OnLvnGetdispinfoDiagAnalogList)
	ON_NOTIFY(NM_DBLCLK,				IDC_DIAG_ANALOG_LIST,	&CDlgTabDiagAnalog::OnNMDblclkAnalogInputList)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTabDiagAnalog message handlers
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::DrawNo( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	if (PortInput.Contact == -1)
	{
		str = _T("Не определен");
		return;
	}

	str.Format(_T("%d"), PortInput.No);
}

void CDlgTabDiagAnalog::DrawContact( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	if (PortInput.Contact == -1)
	{
		str = _T("Не определен");
		return;
	}

	str.Format(_T("%d"), PortInput.Contact);
}


void CDlgTabDiagAnalog::DrawStateDec( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	str.Format(_T("%d"), PortInput.State); 
}

void CDlgTabDiagAnalog::DrawStateHex( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	str.Format(_T("%04Xh"), PortInput.State); 
}

void CDlgTabDiagAnalog::DrawLimit( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	CString formatStr;
	formatStr.Format( _T("%%.%df"), PortInput.Precise );

	str.Format(formatStr + _T(" .. ") + formatStr, PortInput.LowLimit, PortInput.HighLimit); 
}

void CDlgTabDiagAnalog::DrawStateCoversion( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	CString formatStr;
	formatStr.Format( _T("%%.%df"), PortInput.Precise );

	str.Format(formatStr + _T(" %s"),  PortInput.GetCoversionValue(), PortInput.Unit); 
}

void CDlgTabDiagAnalog::DrawDescription( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagAnalog* pThis = (CDlgTabDiagAnalog*)param;
	if (!pThis)
		return;

	ANALOG_INPUT PortInput = pThis->AnalogInputArray[item];

	str = PortInput.Description;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabDiagAnalog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	InitListCtrlColumns( m_List, DiagAnalogColumn, sizeof(DiagAnalogColumn) );

	m_List.SetItemCount((int) AnalogInputArray.GetCount());

	m_List.EnsureVisible( 0, TRUE );
	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	ZeroMemory(&CurrentAnalogInputState, ANALOG_INPUT_COUNT * sizeof(WORD));
	TimerID = (UINT) SetTimer( 1, 250, NULL );

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::OnLvnGetdispinfoDiagAnalogList(NMHDR *pNMHDR, LRESULT *pResult)
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
		DiagAnalogColumn[nColumn].pDrawProc( nItem, nColumn, this, str );
		lstrcpyn(pDispInfo->item.pszText, str.GetBuffer(), str.GetLength() + 1);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::LoadAnalogInputArray()
{
	AnalogInputArray.SetSize(ANALOG_INPUT_COUNT);

	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_ANALOG_INPUT))
		return;

	int nInputCount = (int) AnalogInputArray.GetCount();
	for (int nInput = 0; nInput < ANALOG_INPUT_COUNT; nInput++)
	{
		AnalogInputArray[nInput].No				= nInput + 1;
		AnalogInputArray[nInput].Contact	= AnalogInputContact[nInput];

		Table.ReadRecord(nInput, &AnalogInputArray[nInput]);
	}

	Table.Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::SaveAnalogInputArray()
{
	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_ANALOG_INPUT))
		return;

	Table.Empty();
	
	int nRecordCount = (int) AnalogInputArray.GetCount();
	for(int a = 0; a < nRecordCount ; a++)
		Table.WriteRecord(&AnalogInputArray[a]);
	
	Table.Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::UpdateAnalogInputArray()
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
	{
		m_List.SetItemCount(0);
		return;
	}
	else
	{
		if ( m_List.GetItemCount() != (int) AnalogInputArray.GetCount() )
			m_List.SetItemCount((int) AnalogInputArray.GetCount());
	}

	CString str;
	
	for(int nInput = 0; nInput < ANALOG_INPUT_COUNT; nInput++)
	{
		ANALOG_INPUT PortInput = AnalogInputArray[nInput];

		PortInput.State = pActiveSource->AnalogInputState[nInput];

		AnalogInputArray[nInput] = PortInput;

		if (CurrentAnalogInputState[nInput] != PortInput.State)
		{
			CurrentAnalogInputState[nInput] = PortInput.State;
			m_List.RedrawItems(nInput, nInput);
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::EditAnalogInput()
{
	int nItem = GetSelectedItemIndex(m_List);
	if (nItem == -1)
	{
		AfxMessageBox(_T("Не выбран вход для редактирования!"));
		return;
	}

	ANALOG_INPUT PortInput = AnalogInputArray[nItem];

	CDlgAnalogInput DlgAnalogInput(PortInput);
	if(DlgAnalogInput.DoModal() != IDOK)
		return;

	AnalogInputArray[nItem] = DlgAnalogInput.m_AnalogInput; 

	m_List.RedrawItems(nItem, nItem);

	SaveAnalogInputArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::OnNMDblclkAnalogInputList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nItem = pNMItemActivate->iItem;
	if (nItem == -1)
		return;

	EditAnalogInput();
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_List.m_hWnd)
		m_List.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagAnalog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerID)
		UpdateAnalogInputArray();

	CDialog::OnTimer(nIDEvent);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

// CDlgAnalogInput dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgAnalogInput, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgAnalogInput::CDlgAnalogInput(ANALOG_INPUT AnalogInput, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAnalogInput::IDD, pParent)
	, m_AnalogInput(AnalogInput)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgAnalogInput::~CDlgAnalogInput()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgAnalogInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_ANALOG_INPUT_DESCRIPTION,	str);
		SafeStrncpy(m_AnalogInput.Description, str, ANALOG_INPUT_DESCRIPTION_LEN );	
	}
	else
		DDX_Text(pDX, IDC_ANALOG_INPUT_DESCRIPTION, (CString)m_AnalogInput.Description);

	DDX_Text(pDX, IDC_ANALOG_INPUT_LOW_LIMIT,		m_AnalogInput.LowLimit);
	DDX_Text(pDX, IDC_ANALOG_INPUT_HIGH_LIMIT,	m_AnalogInput.HighLimit);

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_ANALOG_INPUT_UNIT, str);
		SafeStrncpy(m_AnalogInput.Unit, str, ANALOG_INPUT_UNIT_LEN );	
	}
	else
		DDX_Text(pDX, IDC_ANALOG_INPUT_UNIT, (CString)m_AnalogInput.Unit);

	DDX_Text(pDX, IDC_ANALOG_INPUT_PRECISE,		m_AnalogInput.Precise);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgAnalogInput, CDialog)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

