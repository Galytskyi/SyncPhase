#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ColorListCtrl.h"

#include "..\..\INCLUDE\util.h"

// ======================================================================================

const int DISCRETE_INPUT_COUNT			= 8;

const int DiscreteInputContact[DISCRETE_PORT_COUNT][DISCRETE_INPUT_COUNT] = 
{
	{	51,	50,	49,	48,	47,	46,	45,	44}, 		// Порт А
	{	10,	11,	12,	13,	14,	15,	16,	17}, 		// Порт B
	{	35,	36,	37,	38,	39,	40,	41,	42}, 		// Порт C
	{	25,	26,	27,	28,	29,	30,	31,	32}, 		// Порт D
	{	2,	3,	4,	5,	6,	7,	8,	9}, 		// Порт E
	{	61,	60,	59,	58,	57,	56,	55,	54}, 		// Порт F
	{	33,	34,	43,	18,	19,	-1,	-1,	-1}, 		// Порт G
};


// ======================================================================================

const int DISCRETE_INPUT_IO_UNKNOWN	= -1,
					DISCRETE_INPUT_IO_INPUT		= 0,
					DISCRETE_INPUT_IO_OUTPUT		= 1;

const int DISCRETE_INPUT_IO_COUNT		= 2;

const TCHAR* const DiscreteIoStr[DISCRETE_INPUT_IO_COUNT] = 
{
	_T("<- Вход"),
	_T("Выход ->"),
};

// ======================================================================================

const int DISCRETE_INPUT_SIGNALING_TYPE_NONE			= 0,
					DISCRETE_INPUT_SIGNALING_TYPE_BLOCKED	= 1,
					DISCRETE_INPUT_SIGNALING_TYPE_DAMAGE		= 2;

const int DISCRETE_INPUT_SIGNALING_TYPE_COUNT		= 3;


const TCHAR* const DiscreteSignalingTypeStr[DISCRETE_INPUT_SIGNALING_TYPE_COUNT] = 
{
	_T("Нет"),
	_T("Блокировка"),
	_T("Авария"),
};


// ======================================================================================

const int DISCRETE_INPUT_DESCRIPTION_LEN	= 0x50;

struct DISCRETE_INPUT
{
	int			No;
	int			Contact;
	int			Port;
	int			Input;
	int			IO;
	int			State;
	TCHAR		Description[DISCRETE_INPUT_DESCRIPTION_LEN];	
	int			SignalingType;
	
	DISCRETE_INPUT()
	{
		No							= 0;
		Contact					= -1;
		Port						= -1;
		Input						= -1;
		IO							= DISCRETE_INPUT_IO_UNKNOWN;
		State						= 0;
		SafeStrncpy(Description, _T("Резерв"), DISCRETE_INPUT_DESCRIPTION_LEN );	
		SignalingType		= DISCRETE_INPUT_SIGNALING_TYPE_NONE;

	};

	CString GetInputName()
	{
		CString str = _T("Порт ");

		str.AppendChar(65 + Port);
		str.Append(_T(" - "));
		str.AppendChar(49 + Input);
		
		return str;
	};
};


// CDlgTabDiagDiscrete dialog
// ======================================================================================

class CDlgTabDiagDiscrete : public CDialog
{
	DECLARE_DYNAMIC(CDlgTabDiagDiscrete)

public:
	CDlgTabDiagDiscrete(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTabDiagDiscrete();

// Dialog Data
	enum { IDD = IDD_TAB_DIAG_DISCRETE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() { ChangeDiscreteState(); };
	virtual void	OnCancel() {};
public:
	
	static void		DrawNo( int item, int subItem, LPVOID param, CString& str );
	static void		DrawContact( int item, int subItem, LPVOID param, CString& str );
	static void		DrawPort( int item, int subItem, LPVOID param, CString& str );
	static void		DrawIO( int item, int subItem, LPVOID param, CString& str );
	static void		DrawState( int item, int subItem, LPVOID param, CString& str );
	static void		DrawDescription( int item, int subItem, LPVOID param, CString& str );
	static void		DrawSignaling( int item, int subItem, LPVOID param, CString& str );

	CColorListCtrl	m_List;
	
	CArray<DISCRETE_INPUT, DISCRETE_INPUT&> DiscreteInputArray;
	BYTE					CurrentDiscreteInputIO[DISCRETE_PORT_COUNT][DISCRETE_INPUT_COUNT];
	BYTE					CurrentDiscreteInputState[DISCRETE_PORT_COUNT][DISCRETE_INPUT_COUNT];

	UINT					TimerID;

	virtual BOOL	OnInitDialog();
	afx_msg void	OnLvnGetdispinfoDiagDiscreteList(NMHDR *pNMHDR, LRESULT *pResult);

	void					LoadDiscreteInputArray();
	void					SaveDiscreteInputArray();
	void					UpdateDiscreteInputIO();
	void					UpdateDiscreteInputState();

	void					AddMsgInputState(int SourceID, DISCRETE_INPUT& PortInput);

	void					ChangeDiscreteState();
	void					EditDiscreteInput();
	
	afx_msg void	OnNMDblclkDiscreteList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMRClickDiscreteList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
};

// CDlgDiscreteState dialog
// ======================================================================================

const int DISCRETE_STATE_COUNT = 2;

const TCHAR* const DiscreteStateStr[DISCRETE_STATE_COUNT] = 
{
	_T("НЕТ (0)"),
	_T("ДА (1)"),
};

// ======================================================================================

class CDlgDiscreteState : public CDialog
{
	DECLARE_DYNAMIC(CDlgDiscreteState)

public:
	CDlgDiscreteState(BYTE State, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDiscreteState();

// Dialog Data
	enum { IDD = IDD_DISCRETE_STATE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	
	BYTE				m_State;	
	CComboBox		m_cState;

	virtual BOOL OnInitDialog();
};

// ======================================================================================

class CDlgDiscreteInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgDiscreteInput)

public:
	CDlgDiscreteInput(DISCRETE_INPUT DiscreteInput, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDiscreteInput();

// Dialog Data
	enum { IDD = IDD_DISCRETE_INPUT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:

	DISCRETE_INPUT		m_DiscreteInput;

	CComboBox					m_SignalingType;
	virtual BOOL			OnInitDialog();
};

// ======================================================================================