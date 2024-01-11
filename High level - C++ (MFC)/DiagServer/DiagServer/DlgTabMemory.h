#pragma once
#include "afxcmn.h"

#include "..\..\INCLUDE\util.h"
#include "afxwin.h"


// ======================================================================================

const int EEPROM_PARAM_VALUE_TYPE_UNKNOWN	= -1,
					EEPROM_PARAM_VALUE_TYPE_BOOL		= 0,
					EEPROM_PARAM_VALUE_TYPE_BYTE		= 1,
					EEPROM_PARAM_VALUE_TYPE_WORD		= 2;
					
const int EEPROM_PARAM_VALUE_TYPE_COUNT		= 3;

const TCHAR* const MemoryParamValueTypeStr[EEPROM_PARAM_VALUE_TYPE_COUNT] = 
{
	_T("Логический"),
	_T("Числовой (0 - 255)"),
	_T("Числовой (0 - 65535)" ),
};


// ======================================================================================

const int EEPROM_PARAM_TYPE_UNKNOWN				= -1,
					EEPROM_PARAM_TYPE_PARAM					= 0,
					EEPROM_PARAM_TYPE_BLOCK					= 1,
					EEPROM_PARAM_TYPE_SETTING				= 2,
					EEPROM_PARAM_TYPE_DELAY					= 3;

const int EEPROM_PARAM_TYPE_COUNT					= 4;

const TCHAR* const MemoryParamTypeStr[EEPROM_PARAM_TYPE_COUNT] = 
{
	_T("Параметр"),
	_T("Блокировка"),
	_T("Уставка"),
	_T("Задержка"),
};

// ======================================================================================

union EEPROM_PARAM_VALUE
{
	struct
	{
		BYTE Byte_0: 8 ;
		BYTE Byte_1: 8 ;
	};

	WORD	Word;
};

// ======================================================================================

const int EEPROM_PARAM_DESCRIPTION_LEN = 0x50;

struct EEPROM_PARAM
{
	int										ValueType;
	int										Value;
	int										Type;
	TCHAR									Description[EEPROM_PARAM_DESCRIPTION_LEN];	
	
	EEPROM_PARAM()
	{
		ValueType				= EEPROM_PARAM_VALUE_TYPE_BOOL;
		Value						= 0;
		Type						= EEPROM_PARAM_TYPE_PARAM;
		SafeStrncpy(Description, _T("Параметр не используется"), EEPROM_PARAM_DESCRIPTION_LEN );	
	}
};

// ======================================================================================

																					// 16 парамета BOOL		- 2 байта
																					// 4 BYTE							- 4 байта
																					// 1 WORD							- 2 байта
const int EEPROM_PARAM_COUNT							= 21;

const int MemoryParamValueType[EEPROM_PARAM_COUNT] = 
{
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 1
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 2
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 3
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 4
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 5
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 6
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 7
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 8
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 9
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 10
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 11
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 12
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 13
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 14
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 15
	EEPROM_PARAM_VALUE_TYPE_BOOL, // 16
	EEPROM_PARAM_VALUE_TYPE_BYTE, // 1
	EEPROM_PARAM_VALUE_TYPE_BYTE, // 2
	EEPROM_PARAM_VALUE_TYPE_BYTE, // 3
	EEPROM_PARAM_VALUE_TYPE_BYTE, // 4
	EEPROM_PARAM_VALUE_TYPE_WORD, // 1
};


// ======================================================================================


// CDlgTabMemory dialog
// ======================================================================================

class CDlgTabMemory : public CDialog
{
	DECLARE_DYNAMIC(CDlgTabMemory)

public:
	CDlgTabMemory(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTabMemory();

// Dialog Data
	enum { IDD = IDD_TAB_EEPROM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() { ChangeMemoryParamState(); };
	virtual void	OnCancel() {};
public:
	
	static void		DrawParamNo( int item, int subItem, LPVOID param, CString& str );
	static void		DrawTypeValue( int item, int subItem, LPVOID param, CString& str );
	static void		DrawValue( int item, int subItem, LPVOID param, CString& str );
	static void		DrawType( int item, int subItem, LPVOID param, CString& str );
	static void		DrawDescription( int item, int subItem, LPVOID param, CString& str );
		
	CListCtrl			m_List;
	UINT					TimerID;

	CArray<EEPROM_PARAM, EEPROM_PARAM&> MemoryParamArray;
	BYTE					CurrentMemoryByteState[EEPROM_BYTE_COUNT];
	
	virtual BOOL	OnInitDialog();
	afx_msg void	OnLvnGetdispinfoMemoryList(NMHDR *pNMHDR, LRESULT *pResult);

	void					LoadMemoryParamArray();
	void					SaveMemoryParamArray();
	void					UpdateMemoryParamArray();

	void					ChangeMemoryParamState();
	void					EditMemoryParam();
	
	afx_msg void	OnNMDblclkMemoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMRClickMemoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
};


// CDlgMemoryParam dialog
// ======================================================================================

class CDlgMemoryParam : public CDialog
{
	DECLARE_DYNAMIC(CDlgMemoryParam)

public:
	CDlgMemoryParam(EEPROM_PARAM MemoryParam, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMemoryParam();

// Dialog Data
	enum { IDD = IDD_EEPROM_PARAM};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:

	EEPROM_PARAM		m_MemoryParam;
	
	CComboBox				m_MemoryParamType;

	virtual BOOL		OnInitDialog();
	
};


// CDlgMemoryParamState dialog
// ======================================================================================

class CDlgMemoryParamState : public CDialog
{
	DECLARE_DYNAMIC(CDlgMemoryParamState)

public:
	CDlgMemoryParamState(BYTE Byte, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMemoryParamState();

// Dialog Data
	enum { IDD = IDD_EEPROM_PARAM_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	
	BYTE	m_Byte;	
	
	CEdit m_ByteDec;
	CEdit m_ByteHex;

	afx_msg void OnChangeByteDec();
	virtual BOOL OnInitDialog();
};

// ======================================================================================