// DlgDiagServer.h : файл заголовка
//

#pragma once

// ======================================================================================

struct TAB_HEADER
{
	TCHAR*					Description;
	int							nID;
};

const int TAB_SOURCE					= 0,
					TAB_MESSAGE					= 1,
					TAB_DIAG_DISCRETE		= 2,
					TAB_DIAG_ANALOG			= 3,
					TAB_EEPROM					= 4,
					TAB_SOCKET					= 5;

const int TAB_COUNT						= 6; 

const TAB_HEADER TabHeader[TAB_COUNT] =
{
	{_T("Источники"),									IDD_TAB_SOURCES},
	{_T("Собщения"),									IDD_TAB_MESSAGES},
	{_T("Дискретные входы") ,					IDD_TAB_DIAG_DISCRETE},
	{_T("Аналоговые входы") ,					IDD_TAB_DIAG_ANALOG},
	{_T("Энергонезависимая память"),	IDD_TAB_EEPROM},
	{_T("Сетевые соединения"),				IDD_TAB_SOCKETS},
};

// ======================================================================================

const int STATUS_BAR_ITEM_EMPTY						= 0,
					STATUS_BAR_ITEM_CONNECT					= 1,
					STATUS_BAR_ITEM_RECEIVED_BYTES	= 2,
					STATUS_BAR_ITEM_COM							= 3;
					
					
const int STATUS_BAR_ITEM_COUNT						= 4;

// ======================================================================================

// диалоговое окно CDlgDiagServer
class CDlgDiagServer : public CDialog
{
// Создание
public:
	CDlgDiagServer(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_DIAG_SERVER };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV
	

// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnOK() {};
	virtual void OnCancel();

private:
	CTabCtrl						m_TabCtrl;
	CStatusBarCtrl			m_StatusBarCtrl;

public:

	CArray<CDialog*>		m_TabArray;

	void								CreateTabs();
	void								RemoveTabs();
	afx_msg void				OnSelectTab(NMHDR *pNMHDR, LRESULT *pResult);
	void								SelectTab(int nTab);

	afx_msg void				OnCreateSource();
	afx_msg void				OnEditSource();
	afx_msg void				OnDeleteSource();

	afx_msg void				OnStartRecvMsg();
	afx_msg void				OnStopRecvMsg();
	afx_msg void				OnMsgArrayClear();
	afx_msg void				OnAutoEnsureMsg();
	
	afx_msg void				OnEditDiscreteInput();
	afx_msg void				OnChangeDiscreteState();
	afx_msg void				OnUpdateDiscreteIO();
	afx_msg void				OnEditAnalogInput();

	afx_msg void				OnEditMemoryParam();
	afx_msg void				OnChangeMemoryParamState();
	afx_msg void				OnUpdateMemoryParam();

	afx_msg void				OnReport();
	afx_msg void				OnOptions();

	void								UpadateMenu();
	void								InitStatusBar();
	void								UpdateStatusBar();

	afx_msg void				OnSize(UINT nType, int cx, int cy);
};
