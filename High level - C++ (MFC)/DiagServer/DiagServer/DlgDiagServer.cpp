
// DlgDiagServer.cpp : файл реализации
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgDiagServer.h"

#include "DlgTabSource.h"
#include "DlgTabMessage.h"
#include "DlgTabDiagDiscrete.h"
#include "DlgTabDiagAnalog.h"
#include "DlgTabMemory.h"
#include "DlgTabSocket.h"
#include "DlgReport.h"
#include "DlgOptions.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CDlgAbout используется для описания сведений о приложении
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

class CDlgAbout : public CDialog
{
public:
	CDlgAbout();

// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic				m_Version;
	virtual BOOL	OnInitDialog();
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgAbout::CDlgAbout() : CDialog(CDlgAbout::IDD)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_COMMAND_VERSION, m_Version);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
END_MESSAGE_MAP()


BOOL CDlgAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str, strVersion;

#ifdef _DEBUG
	//str.Format("Сборка:\t\t\t\t%d (DEBUG)\n", BUILD_NO_DEBUG);
#else
	//str.Format("Сборка:\t\t\t\t%d (RELEASE)\n", BUILD_NO_RELEASE);	
#endif

	strVersion.Append(_T("Дата сборки:\t"));
	str = __DATE__;
	strVersion.Append(str);
	strVersion.Append(_T("\n"));

	strVersion.Append(_T("Время сборки:\t"));
	str = __TIME__;
	strVersion.Append(str);
	strVersion.Append(_T("\n"));
	
	m_Version.SetWindowText(strVersion);

	return TRUE;  
}


// диалоговое окно CDlgDiagServer
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

static CRect rDlgRect;

static UINT Indicators[] =
{
	ID_SEPARATOR,
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgDiagServer::CDlgDiagServer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDiagServer::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CDlgDiagServer, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB,					&CDlgDiagServer::OnSelectTab)
	// Источники
	ON_COMMAND(ID_CREATE_SOURCE,							&CDlgDiagServer::OnCreateSource)
	ON_COMMAND(ID_EDIT_SOURCE,								&CDlgDiagServer::OnEditSource)
	ON_COMMAND(ID_DELETE_SOURCE,							&CDlgDiagServer::OnDeleteSource)
	// Сообщения
	ON_COMMAND(ID_START_RECV_MSG,							&CDlgDiagServer::OnStartRecvMsg)
	ON_COMMAND(ID_STOP_RECV_MSG,							&CDlgDiagServer::OnStopRecvMsg)
	ON_COMMAND(ID_MSG_ARRAY_CLEAR,						&CDlgDiagServer::OnMsgArrayClear)
	ON_COMMAND(ID_AUTO_ENSURE_MSG,						&CDlgDiagServer::OnAutoEnsureMsg)
	
	// Входы
	ON_COMMAND(ID_EDIT_DISCRETE_INPUT,				&CDlgDiagServer::OnEditDiscreteInput)
	ON_COMMAND(ID_CHANGE_DISCRETE_STATE,			&CDlgDiagServer::OnChangeDiscreteState)
	ON_COMMAND(ID_UPDATE_DISCRETE_IO,					&CDlgDiagServer::OnUpdateDiscreteIO)
	ON_COMMAND(ID_EDIT_ANALOG_INPUT,					&CDlgDiagServer::OnEditAnalogInput)
	// Энергонезависимая память
	ON_COMMAND(ID_EDIT_EEPROM_PARAM,					&CDlgDiagServer::OnEditMemoryParam)
	ON_COMMAND(ID_CHANGE_EEPROM_PARAM_STATE,	&CDlgDiagServer::OnChangeMemoryParamState)
	ON_COMMAND(ID_UPDATE_EEPROM_PARAM,				&CDlgDiagServer::OnUpdateMemoryParam)
	// Вид
	ON_COMMAND(ID_REPORT,											&CDlgDiagServer::OnReport)
	ON_COMMAND(ID_OPTIONS,										&CDlgDiagServer::OnOptions)
	//
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()


// обработчики сообщений CDlgDiagServer
// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgDiagServer::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch(theApp.AppType)
	{
		case APP_TYPE_SERVER: SetWindowText(_T("Диагностический сервер")); break;
		case APP_TYPE_CLIENT: SetWindowText(_T("Диагностический клиент")); break;
	}

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	m_TabCtrl.GetWindowRect(&rDlgRect);
	ScreenToClient(&rDlgRect);
		
	UpadateMenu();
	InitStatusBar();
	CreateTabs();
	

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CDlgAbout dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
// -------------------------------------------------------------------------------------------------------------------------------------------------
HCURSOR CDlgDiagServer::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::CreateTabs()
{
	CDialog* pDlg	=	NULL;
	for(int t = 0; t < TAB_COUNT; t++)
	{
		m_TabCtrl.InsertItem( t, TabHeader[t].Description );
				
		switch(t)
		{
			case TAB_SOURCE:				pDlg = new CDlgTabSource;				break;
			case TAB_MESSAGE:				pDlg = new CDlgTabMessage;			break;
			case TAB_DIAG_DISCRETE:	pDlg = new CDlgTabDiagDiscrete;	break;
			case TAB_DIAG_ANALOG:		pDlg = new CDlgTabDiagAnalog;		break;
			case TAB_EEPROM:				pDlg = new CDlgTabMemory;				break;
			case TAB_SOCKET:				pDlg = new CDlgTabSocket;				break;
		}
		if (pDlg)
		{
			pDlg->Create( TabHeader[t].nID, this );
			m_TabArray.Add(pDlg);
		}
	}

	SelectTab(0);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::RemoveTabs()
{
	int nTabCount = (int) m_TabArray.GetCount();
	for(int t = nTabCount - 1;  t >= 0; t--)
	{
		CDialog* pTabDlg = m_TabArray[t];
		if(!pTabDlg)
			continue;

		pTabDlg->DestroyWindow();
		delete pTabDlg;

		m_TabArray.RemoveAt(t);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnSelectTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nTab = m_TabCtrl.GetCurSel();
	if (nTab == CB_ERR)
		return;

	SelectTab(nTab);
	
	*pResult = 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::SelectTab(int nTab)
{
	int nTabCount = (int) m_TabArray.GetCount();
	for(int t = 0; t < nTabCount; t++)
	{
		CDialog* pTabDlg = m_TabArray[t];
		if(!pTabDlg)
			continue;

		pTabDlg->ShowWindow(SW_HIDE);
	}

	if (nTab < 0 || nTab >= nTabCount)
		return;

	CDialog* pDlg	=	m_TabArray[nTab];
	if (!pDlg)
		return;

	pDlg->SetWindowPos(	NULL, 
											rDlgRect.left + 3, rDlgRect.top + 22, 
											rDlgRect.right-rDlgRect.left - 8, rDlgRect.bottom-rDlgRect.top - 25, 
											SWP_NOZORDER);
	pDlg->ShowWindow(SW_SHOW);

	UpadateMenu();

	//switch(nTab)
	//{
	//	case TAB_DIAG_DISCRETE:	OnUpdateDiscreteIO();		break;
	//	case TAB_EEPROM:				OnUpdateMemoryParam();	break;
	//}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnCreateSource()
{
	CDlgTabSource* pTabSources = (CDlgTabSource*) m_TabArray[TAB_SOURCE];
	if (!pTabSources)
		return;

	pTabSources->CreateSource();

	UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnEditSource()
{
	CDlgTabSource* pTabSources = (CDlgTabSource*) m_TabArray[TAB_SOURCE];
	if (!pTabSources)
		return;

	pTabSources->EditSource();

	UpadateMenu();
	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnDeleteSource()
{
	CDlgTabSource* pTabSources = (CDlgTabSource*) m_TabArray[TAB_SOURCE];
	if (!pTabSources)
		return;

	pTabSources->DeleteSource();

	UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnStartRecvMsg()
{
	if (!theApp.SourceBase.GetActiveSource())
		return;

	theApp.SourceBase.GetActiveSource()->EnableRecvMsg = TRUE;
	
	UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnStopRecvMsg()
{
	if (!theApp.SourceBase.GetActiveSource())
		return;

	theApp.SourceBase.GetActiveSource()->EnableRecvMsg = FALSE;

	UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnMsgArrayClear()
{
	if (!theApp.SourceBase.GetActiveSource())
		return;

	theApp.SourceBase.GetActiveSource()->MsgStrArray.RemoveAll();

	UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnAutoEnsureMsg()
{
	if (!theApp.SourceBase.GetActiveSource())
		return;

	theApp.SourceBase.GetActiveSource()->AutoEnsureMsg = !theApp.SourceBase.GetActiveSource()->AutoEnsureMsg;

	UpadateMenu();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnEditDiscreteInput()
{
	CDlgTabDiagDiscrete* pDlgTabDiagDiscrete = (CDlgTabDiagDiscrete*) m_TabArray[TAB_DIAG_DISCRETE];
	if (!pDlgTabDiagDiscrete)
		return;

	pDlgTabDiagDiscrete->EditDiscreteInput();

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnChangeDiscreteState()
{
	CDlgTabDiagDiscrete* pDlgTabDiagDiscrete = (CDlgTabDiagDiscrete*) m_TabArray[TAB_DIAG_DISCRETE];
	if (!pDlgTabDiagDiscrete)
		return;

	pDlgTabDiagDiscrete->ChangeDiscreteState();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnUpdateDiscreteIO()
{
	CDlgTabDiagDiscrete* pDlgTabDiagDiscrete = (CDlgTabDiagDiscrete*) m_TabArray[TAB_DIAG_DISCRETE];
	if (!pDlgTabDiagDiscrete)
		return;

	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
		return;
	
	switch(theApp.AppType)
	{
		case APP_TYPE_SERVER:
			if (pActiveSource && pActiveSource->pCommThread)
				pActiveSource->pCommThread->RequestGetPortIO();
			break;
		case APP_TYPE_CLIENT:
			if (pActiveSource && theApp.pSocketThread)
				theApp.pSocketThread->RequestGetPortIO(pActiveSource->ID);

			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnEditAnalogInput()
{
	CDlgTabDiagAnalog* pDlgTabDiagAnalog = (CDlgTabDiagAnalog*) m_TabArray[TAB_DIAG_ANALOG];
	if (!pDlgTabDiagAnalog)
		return;

	pDlgTabDiagAnalog->EditAnalogInput();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnEditMemoryParam()
{
	CDlgTabMemory* pDlgTabMemory = (CDlgTabMemory*) m_TabArray[TAB_EEPROM];
	if (!pDlgTabMemory)
		return;

	pDlgTabMemory->EditMemoryParam();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnChangeMemoryParamState()
{
	CDlgTabMemory* pDlgTabMemory = (CDlgTabMemory*) m_TabArray[TAB_EEPROM];
	if (!pDlgTabMemory)
		return;

	pDlgTabMemory->ChangeMemoryParamState();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnUpdateMemoryParam()
{
	CDlgTabMemory* pDlgTabMemory = (CDlgTabMemory*) m_TabArray[TAB_EEPROM];
	if (!pDlgTabMemory)
		return;
	
	if (theApp.SourceBase.GetActiveSource() && theApp.SourceBase.GetActiveSource()->pCommThread)
		theApp.SourceBase.GetActiveSource()->pCommThread->RequestGetMemoryParam();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnReport()
{
	int ReportType	= -1;
	int TabItem			= m_TabCtrl.GetCurSel(); 

	switch(TabItem)
	{
		case TAB_SOURCE:				ReportType = REPORT_TYPE_DIAG_SOURCE;			break;
		case TAB_MESSAGE:				ReportType = REPORT_TYPE_DIAG_MESSAGE;		break;
		case TAB_DIAG_DISCRETE:	ReportType = REPORT_TYPE_DISCRETE_INPUT;	break;
		case TAB_DIAG_ANALOG:		ReportType = REPORT_TYPE_ANALOG_INPUT;		break;
		case TAB_EEPROM:				ReportType = REPORT_TYPE_EEPROM_PARAM;		break;
	};
	
	if(ReportType == -1)
		return;
	
	CDlgReport DlgReport(ReportType);
	DlgReport.DoModal();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnOptions()
{
	CDlgOptions DlgOptions;
	DlgOptions.DoModal();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::UpadateMenu()
{
	CMenu* pMainMenu = AfxGetMainWnd()->GetMenu();
	if (!pMainMenu)
		return;

	CMenu* pSubMenu0 = pMainMenu->GetSubMenu(0);
	CMenu* pSubMenu1 = pMainMenu->GetSubMenu(1);
	if (!pSubMenu0 || !pSubMenu1)
		return;

	UINT SourceEdit;
	UINT SourceDelete;

	UINT EnableRecvMsgOn;
	UINT EnableRecvMsgOff;
	UINT MsgArrayRemoveAll;
	UINT AutoEnsureMsg;
	UINT AutoEnsureMsgCheck;

	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
	{
		SourceEdit					= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		SourceDelete				= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;

		EnableRecvMsgOn			= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		EnableRecvMsgOff		= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		MsgArrayRemoveAll		= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		AutoEnsureMsg				= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		AutoEnsureMsgCheck	= MF_BYCOMMAND | MF_UNCHECKED;
	}
	else
	{
		SourceEdit					= MF_BYCOMMAND | MF_ENABLED;
		SourceDelete				= MF_BYCOMMAND | MF_ENABLED;

		if (pActiveSource->EnableRecvMsg)
		{
			EnableRecvMsgOn		= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
			EnableRecvMsgOff	= MF_BYCOMMAND | MF_ENABLED;
		}
		else
		{
			EnableRecvMsgOn		= MF_BYCOMMAND | MF_ENABLED;
			EnableRecvMsgOff	= MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		}

		MsgArrayRemoveAll		= MF_BYCOMMAND | MF_ENABLED;
		
		AutoEnsureMsg				= MF_BYCOMMAND | MF_ENABLED;
		if (pActiveSource->AutoEnsureMsg)
			AutoEnsureMsgCheck	= MF_BYCOMMAND | MF_CHECKED;
		else
			AutoEnsureMsgCheck	= MF_BYCOMMAND | MF_UNCHECKED;

	}

	pSubMenu0->EnableMenuItem(ID_EDIT_SOURCE, SourceEdit);
	pSubMenu0->EnableMenuItem(ID_DELETE_SOURCE, SourceDelete);

	pSubMenu1->EnableMenuItem(ID_START_RECV_MSG, EnableRecvMsgOn);
	pSubMenu1->EnableMenuItem(ID_STOP_RECV_MSG, EnableRecvMsgOff);
	pSubMenu1->EnableMenuItem(ID_MSG_ARRAY_CLEAR, MsgArrayRemoveAll);
	pSubMenu1->EnableMenuItem(ID_AUTO_ENSURE_MSG, AutoEnsureMsg);
	pSubMenu1->CheckMenuItem(ID_AUTO_ENSURE_MSG, AutoEnsureMsgCheck);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::InitStatusBar()
{
	int StatusBarItemWidth[STATUS_BAR_ITEM_COUNT];
	StatusBarItemWidth[STATUS_BAR_ITEM_EMPTY]						= rDlgRect.right - 350;
	StatusBarItemWidth[STATUS_BAR_ITEM_CONNECT]					= rDlgRect.right - 200;
	StatusBarItemWidth[STATUS_BAR_ITEM_RECEIVED_BYTES]	= rDlgRect.right - 70;
	StatusBarItemWidth[STATUS_BAR_ITEM_COM]							= rDlgRect.right;

	m_StatusBarCtrl.Create(WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | SBARS_SIZEGRIP, CRect(0,0,0,0), this, AFX_IDW_STATUS_BAR);
	m_StatusBarCtrl.SetParts(STATUS_BAR_ITEM_COUNT, (int*) &StatusBarItemWidth);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::UpdateStatusBar()
{
	CSourceItem* pActiveSource = theApp.SourceBase.GetActiveSource();
	if (!pActiveSource)
	{
		for(int p = 0; p < STATUS_BAR_ITEM_COUNT; p++)
			m_StatusBarCtrl.SetText(_T(""), p, 0);
		return;
	}

	CString strConnect, strReceivedBytes, strComm;

	switch(theApp.AppType)
	{
		case APP_TYPE_SERVER: 
		
			int SocketCount;
			SocketCount = theApp.ClientSocketBase.GetCount();
			
			strConnect= _T("Сетевых соединений: "); 
			strConnect.AppendFormat( !SocketCount ? _T("Нет") : _T("%d"), SocketCount); 
			
			break;

		case APP_TYPE_CLIENT: 

			if (!theApp.pSocketThread)
			{
				strConnect = _T("Сокет не создан"); 
				break;
			}

			strConnect = _T("Сетевое соединение:"); 
			strConnect.Append( theApp.pSocketThread->IsConnect() ? _T(" Есть") : _T(" Нет")); 
			
			break;
	}

	strReceivedBytes = !pActiveSource->pCommThread ? _T("") : _T(" Принято: ") + pActiveSource->pCommThread->CommState.GetReceivedBytesStr();
	strComm.Format(_T(" %s"), pActiveSource->Comm);
	
	m_StatusBarCtrl.SetText(strConnect,					STATUS_BAR_ITEM_CONNECT,				0);
	m_StatusBarCtrl.SetText(strReceivedBytes,		STATUS_BAR_ITEM_RECEIVED_BYTES, 0);
	m_StatusBarCtrl.SetText(strComm,						STATUS_BAR_ITEM_COM,						0);
	
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//if (m_TabCtrl.m_hWnd)
	//{
	//	rDlgRect.right = cx - 20;
	//	rDlgRect.bottom = cy - 20;
	//	m_TabCtrl.SetWindowPos( NULL, rDlgRect.left, rDlgRect.top, rDlgRect.right, rDlgRect.bottom, SWP_NOZORDER );	

	//}
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgDiagServer::OnCancel()
{
	RemoveTabs();

	CDialog::OnCancel();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------



