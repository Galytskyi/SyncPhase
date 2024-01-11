// DlgTabDiagDiscrete.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgTabDiagDiscrete.h"


#include "DataFile.h"

// CDlgTabDiagDiscrete dialog

// -------------------------------------------------------------------------------------------------------------------------------------------------

COLUMN DiagDiscreteColumn[] = 
{
	{	_T("N"),						30,		LVCFMT_CENTER,	CDlgTabDiagDiscrete::DrawNo						},
	{	_T("Ножка"),				50,		LVCFMT_CENTER,	CDlgTabDiagDiscrete::DrawContact			},
	{	_T("Порт"),					70,		LVCFMT_CENTER,	CDlgTabDiagDiscrete::DrawPort					},
	{	_T("Вх / Вых"),			90,		LVCFMT_CENTER,	CDlgTabDiagDiscrete::DrawIO						},
	{	_T("Состсояние"),		100,	LVCFMT_CENTER,	CDlgTabDiagDiscrete::DrawState				},
	{	_T("Описание"),			250,	LVCFMT_LEFT,		CDlgTabDiagDiscrete::DrawDescription	},
	{	_T("Сигнализация"),	100,	LVCFMT_CENTER,	CDlgTabDiagDiscrete::DrawSignaling		},
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgTabDiagDiscrete, CDialog)

CDlgTabDiagDiscrete::CDlgTabDiagDiscrete(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTabDiagDiscrete::IDD, pParent)
{
	LoadDiscreteInputArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgTabDiagDiscrete::~CDlgTabDiagDiscrete()
{
	SaveDiscreteInputArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DIAG_DISCRETE_LIST, m_List);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgTabDiagDiscrete, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO,	IDC_DIAG_DISCRETE_LIST,	&CDlgTabDiagDiscrete::OnLvnGetdispinfoDiagDiscreteList)
	ON_NOTIFY(NM_DBLCLK,				IDC_DIAG_DISCRETE_LIST,	&CDlgTabDiagDiscrete::OnNMDblclkDiscreteList)
	ON_NOTIFY(NM_RCLICK,				IDC_DIAG_DISCRETE_LIST,	&CDlgTabDiagDiscrete::OnNMRClickDiscreteList)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTabDiagDiscrete message handlers
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::DrawNo( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;

	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}

	str.Format(_T("%d"), PortInput.No);
}


void CDlgTabDiagDiscrete::DrawContact( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;

	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}

	str.Format(_T("%d"), PortInput.Contact);
}

void CDlgTabDiagDiscrete::DrawPort( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;

	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}

	str = PortInput.GetInputName();
}

void CDlgTabDiagDiscrete::DrawIO( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;
	
	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}
	
	if (PortInput.IO < 0 || PortInput.IO >= DISCRETE_INPUT_IO_COUNT)
	{
		str = _T("Не определен");
		return;
	}

	str = DiscreteIoStr[PortInput.IO];
}

void CDlgTabDiagDiscrete::DrawState( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;

	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}

	str.Format(_T("%s (%d)"), PortInput.State ? _T("ДА") : _T("НЕТ"), PortInput.State); 
}

void CDlgTabDiagDiscrete::DrawDescription( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;

	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}

	str = PortInput.Description;
}

void CDlgTabDiagDiscrete::DrawSignaling( int item, int subItem, LPVOID param, CString& str )
{
	CDlgTabDiagDiscrete* pThis = (CDlgTabDiagDiscrete*)param;
	if (!pThis)
		return;

	DISCRETE_INPUT PortInput = pThis->DiscreteInputArray[item];
	if (PortInput.Input == -1 || PortInput.Contact == -1)
	{
		str = _T("");
		return;
	}

	if (PortInput.SignalingType < 0  || PortInput.SignalingType >= DISCRETE_INPUT_SIGNALING_TYPE_COUNT)
	{
		str = _T("");
		return;
	}

	str = DiscreteSignalingTypeStr[PortInput.SignalingType];
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgTabDiagDiscrete::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	InitListCtrlColumns( m_List, DiagDiscreteColumn, sizeof(DiagDiscreteColumn) );

	m_List.SetItemCount((int) DiscreteInputArray.GetCount());

	m_List.EnsureVisible( 0, TRUE );
	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	ZeroMemory(&CurrentDiscreteInputIO, DISCRETE_PORT_COUNT * DISCRETE_INPUT_COUNT * sizeof(BYTE));
	ZeroMemory(&CurrentDiscreteInputState, DISCRETE_PORT_COUNT * DISCRETE_INPUT_COUNT * sizeof(BYTE));
	TimerID = (UINT) SetTimer( 1, 250, NULL );

	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::OnLvnGetdispinfoDiagDiscreteList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	int nItem = pDispInfo->item.iItem;
	int nColumn = pDispInfo->item.iSubItem;

	if (nItem == -1 || nColumn == -1)
		return;

	if (pDispInfo->item.mask & LVIF_INDENT)
	{
		if (DiscreteInputArray[nItem].State)
		{
			switch(DiscreteInputArray[nItem].SignalingType)
			{
				case DISCRETE_INPUT_SIGNALING_TYPE_BLOCKED:	pDispInfo->item.iIndent = RGB(0xFF, 0xFF, 0xCC);	break;
				case DISCRETE_INPUT_SIGNALING_TYPE_DAMAGE:	pDispInfo->item.iIndent = RGB(0xFA, 0xD0, 0xD0);	break;
			}
		}
	}

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString str;
		DiagDiscreteColumn[nColumn].pDrawProc( nItem, nColumn, this, str );
		lstrcpyn(pDispInfo->item.pszText, str.GetBuffer(), str.GetLength() + 1);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::LoadDiscreteInputArray()
{
	DiscreteInputArray.SetSize(DISCRETE_PORT_COUNT * (DISCRETE_INPUT_COUNT + 1));

	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_DISCRETE_INPUT))
		return;

	int No = 1;

	for(int nPort = 0; nPort < DISCRETE_PORT_COUNT; nPort++)
	{
		for(int nInput = 0; nInput < DISCRETE_INPUT_COUNT + 1; nInput++)
		{
			if (nInput == DISCRETE_INPUT_COUNT)
				continue;

			int nItem = (nPort*(DISCRETE_INPUT_COUNT+1)) + nInput;

			DiscreteInputArray[nItem].No			= No++;
			DiscreteInputArray[nItem].Contact	= DiscreteInputContact[nPort][nInput];

			DiscreteInputArray[nItem].Port		= nPort;
			DiscreteInputArray[nItem].Input		= nInput;

			Table.ReadRecord(nItem, &DiscreteInputArray[nItem]);
		}
	}

	Table.Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::SaveDiscreteInputArray()
{
	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_DISCRETE_INPUT))
		return;

	Table.Empty();
	
	int nRecordCount = (int) DiscreteInputArray.GetCount();
	for(int d = 0; d < nRecordCount ; d++)
		Table.WriteRecord(&DiscreteInputArray[d]);
	
	Table.Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::UpdateDiscreteInputIO()
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;

	CString str;
	
	for(int nPort = 0; nPort < DISCRETE_PORT_COUNT; nPort++)
	{
		for(int nInput = 0; nInput < DISCRETE_INPUT_COUNT + 1; nInput++)
		{
			if (nInput == DISCRETE_INPUT_COUNT)
				continue;

			int nItem = (nPort*(DISCRETE_INPUT_COUNT+1)) + nInput;

			DISCRETE_INPUT PortInput = DiscreteInputArray[nItem];

			PortInput.IO = (pActiveSource->PortIO[nPort] & (1<<nInput)) != 0;

			DiscreteInputArray[nItem] = PortInput;

			if (CurrentDiscreteInputIO[nPort][nInput] != PortInput.IO)
			{
				CurrentDiscreteInputIO[nPort][nInput] = PortInput.IO;
				m_List.RedrawItems(nItem, nItem);
			}
			
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::UpdateDiscreteInputState()
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;

	CString str;
	
	for(int nPort = 0; nPort < DISCRETE_PORT_COUNT; nPort++)
	{
		for(int nInput = 0; nInput < DISCRETE_INPUT_COUNT + 1; nInput++)
		{
			if (nInput == DISCRETE_INPUT_COUNT)
				continue;

			int nItem = (nPort*(DISCRETE_INPUT_COUNT+1)) + nInput;

			DISCRETE_INPUT PortInput = DiscreteInputArray[nItem];

			PortInput.State = (pActiveSource->DiscretePortState[nPort] & (1<<nInput)) != 0;

			DiscreteInputArray[nItem] = PortInput;

			if (CurrentDiscreteInputState[nPort][nInput] != PortInput.State)
			{
				CurrentDiscreteInputState[nPort][nInput] = PortInput.State;
				m_List.RedrawItems(nItem, nItem);

				//AddMsgInputState(pActiveSource->ID, PortInput); Временно коментируем
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::AddMsgInputState(int SourceID, DISCRETE_INPUT& PortInput)
{
	if (SourceID == -1)
		return;

	CString TimeStr;
	
	DIAG_MSG_INPUT DiagMsg;

	DiagMsg.SourceID = SourceID;

	GetLocalTime(&DiagMsg.LocalTime);
		
	TimeStr.Format(	_T("%02d:%02d:%02d.%03d"), 
									DiagMsg.LocalTime.wHour, DiagMsg.LocalTime.wMinute, DiagMsg.LocalTime.wSecond, DiagMsg.LocalTime.wMilliseconds);           
	SafeStrncpy(DiagMsg.TimeStr, TimeStr, DIAG_MSG_TIME_LEN );	
	

	TimeStr.Format(	_T("%02d-%02d-%04d"), DiagMsg.LocalTime.wDay,  DiagMsg.LocalTime.wMonth, DiagMsg.LocalTime.wYear);           
	SafeStrncpy(DiagMsg.DateStr, TimeStr, DIAG_MSG_DATE_LEN );

	DiagMsg.dwTimeCode.SetTime(DiagMsg.LocalTime);
	DiagMsg.dwTimeCode.SetDate(DiagMsg.LocalTime);

	DiagMsg.Contact = PortInput.Contact;
	
	DiagMsg.State = PortInput.State;

	CDataFile MsgTable;
	if (MsgTable.Open(TABLE_TYPE_DIAG_MSG_INPUT))
	{
		MsgTable.WriteRecord(&DiagMsg);
		MsgTable.Close();
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::ChangeDiscreteState()
{
	int nItem = GetSelectedItemIndex(m_List);
	if (nItem == -1)
		return;

	DISCRETE_INPUT PortInput = DiscreteInputArray[nItem];
	if (PortInput.IO == DISCRETE_INPUT_IO_INPUT ||  PortInput.Input == -1)
		return;

	CDlgDiscreteState DlgDiscreteState(PortInput.State);
	if(DlgDiscreteState.DoModal() != IDOK)
		return;

	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource || !pActiveSource->pCommThread)
		return;

	pActiveSource->pCommThread->RequestSetDiscreteState(PortInput.Port, PortInput.Input, DlgDiscreteState.m_State);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::EditDiscreteInput()
{
	int nItem = GetSelectedItemIndex(m_List);
	if (nItem == -1)
	{
		AfxMessageBox(_T("Не выбран вход для редактирования!"));
		return;
	}

	DISCRETE_INPUT PortInput = DiscreteInputArray[nItem];

	CDlgDiscreteInput DlgDiscreteInput(PortInput);
	if(DlgDiscreteInput.DoModal() != IDOK)
		return;

	DiscreteInputArray[nItem] = DlgDiscreteInput.m_DiscreteInput; 

	m_List.RedrawItems(nItem, nItem);

	SaveDiscreteInputArray();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::OnNMDblclkDiscreteList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nItem = pNMItemActivate->iItem;
	if (nItem == -1)
		return;

	ChangeDiscreteState();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::OnNMRClickDiscreteList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

   CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	 if (!pMainMenu)
		 return;

   CMenu* pSubMenu = pMainMenu->GetSubMenu(2);    
	 if (!pSubMenu)
		 return;
   
   CMenu* pSubMenuD = pSubMenu->GetSubMenu(0);    
	 if (!pSubMenuD)
		 return;

   ClientToScreen(&pNMItemActivate->ptAction);
   pSubMenuD->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, AfxGetMainWnd());
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_List.m_hWnd)
		m_List.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER );	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgTabDiagDiscrete::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerID)
	{
		UpdateDiscreteInputIO();
		UpdateDiscreteInputState();
	}

	CDialog::OnTimer(nIDEvent);
}

// CDlgDiscreteState dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgDiscreteState, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgDiscreteState::CDlgDiscreteState(BYTE State, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDiscreteState::IDD, pParent)
	, m_State(!State)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgDiscreteState::~CDlgDiscreteState()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiscreteState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISCRETE_STATE, m_cState);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgDiscreteState, CDialog)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgDiscreteState::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int s = 0; s < DISCRETE_STATE_COUNT; s++)
		m_cState.AddString(DiscreteStateStr[s]);

	m_cState.SetCurSel(m_State);

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------


void CDlgDiscreteState::OnOK()
{
	m_State = m_cState.GetCurSel();

	CDialog::OnOK();
}

// CDlgDiscreteInput dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgDiscreteInput, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgDiscreteInput::CDlgDiscreteInput(DISCRETE_INPUT DiscreteInput, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDiscreteInput::IDD, pParent)
	, m_DiscreteInput(DiscreteInput)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgDiscreteInput::~CDlgDiscreteInput()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiscreteInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if (pDX->m_bSaveAndValidate)
	{
		CString str;
		DDX_Text(pDX, IDC_DISCRETE_INPUT_DESCRIPTION,	str);
		SafeStrncpy(m_DiscreteInput.Description, str, DISCRETE_INPUT_DESCRIPTION_LEN );	
	}
	else
		DDX_Text(pDX, IDC_DISCRETE_INPUT_DESCRIPTION,	(CString)m_DiscreteInput.Description);

	DDX_Control(pDX, IDC_DISCRETE_INPUT_SIGNALING_TYPE, m_SignalingType);
	DDX_CBIndex(pDX, IDC_DISCRETE_INPUT_SIGNALING_TYPE, m_DiscreteInput.SignalingType);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgDiscreteInput, CDialog)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgDiscreteInput::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int t = 0; t < DISCRETE_INPUT_SIGNALING_TYPE_COUNT; t++)
		m_SignalingType.AddString(DiscreteSignalingTypeStr[t]);
	m_SignalingType.SetCurSel(m_DiscreteInput.SignalingType);
	
	return TRUE;  
}

// -------------------------------------------------------------------------------------------------------------------------------------------------