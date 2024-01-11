#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "..\..\INCLUDE\util.h"

// ======================================================================================

const int AnalogInputContact[8] = 
{
	61,	60,	59,	58,	57,	56,	55,	54, 		// Порт F
};

const int ANALOG_INPUT_DESCRIPTION_LEN	= 0x50;
const int ANALOG_INPUT_UNIT_LEN					= 0x10;

struct ANALOG_INPUT
{
	int			No;
	int			Contact;
	WORD		State;
	TCHAR		Description[ANALOG_INPUT_DESCRIPTION_LEN];	
	int			Precise;
	double	LowLimit;
	double	HighLimit;
	TCHAR		Unit[ANALOG_INPUT_UNIT_LEN];
	
	ANALOG_INPUT()
	{
		No							= 0;
		Contact					= -1;
		State						= 0;
		Description[0]	= 0;	
		SafeStrncpy(Description, _T("Резерв"), ANALOG_INPUT_DESCRIPTION_LEN );	
		Precise					= 0;
		LowLimit				= 0;
		HighLimit				= 1023;
		Unit[0]					= 0;
	};

	double GetCoversionValue() { return (State*(HighLimit - LowLimit))/1023.0 + LowLimit; };
};

// CDlgTabDiagAnalog dialog
// ======================================================================================

class CDlgTabDiagAnalog : public CDialog
{
	DECLARE_DYNAMIC(CDlgTabDiagAnalog)

public:
	CDlgTabDiagAnalog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTabDiagAnalog();

// Dialog Data
	enum { IDD = IDD_TAB_DIAG_ANALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() { EditAnalogInput(); };
	virtual void	OnCancel() {};
public:
	
	static void		DrawNo( int item, int subItem, LPVOID param, CString& str );
	static void		DrawContact( int item, int subItem, LPVOID param, CString& str );
	static void		DrawStateDec( int item, int subItem, LPVOID param, CString& str );
	static void		DrawStateHex( int item, int subItem, LPVOID param, CString& str );
	static void		DrawLimit( int item, int subItem, LPVOID param, CString& str );
	static void		DrawStateCoversion( int item, int subItem, LPVOID param, CString& str );
	static void		DrawDescription( int item, int subItem, LPVOID param, CString& str );

	CListCtrl			m_List;
	
	CArray<ANALOG_INPUT, ANALOG_INPUT&> AnalogInputArray;
	WORD					CurrentAnalogInputState[ANALOG_INPUT_COUNT];

	UINT					TimerID;

	virtual BOOL	OnInitDialog();
	afx_msg void	OnLvnGetdispinfoDiagAnalogList(NMHDR *pNMHDR, LRESULT *pResult);

	void					LoadAnalogInputArray();
	void					SaveAnalogInputArray();
	void					UpdateAnalogInputArray();
	
	void					EditAnalogInput();

	afx_msg void	OnNMDblclkAnalogInputList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
};

// ======================================================================================

class CDlgAnalogInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgAnalogInput)

public:
	CDlgAnalogInput(ANALOG_INPUT AnalogInput, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAnalogInput();

// Dialog Data
	enum { IDD = IDD_ANALOG_INPUT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:

	ANALOG_INPUT		m_AnalogInput;
};

// ======================================================================================