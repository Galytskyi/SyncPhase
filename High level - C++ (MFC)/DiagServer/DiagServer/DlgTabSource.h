#pragma once
#include "afxcmn.h"
#include "ColorListCtrl.h"


// CDlgTabSource dialog
// ======================================================================================

class CDlgTabSource : public CDialog
{
	DECLARE_DYNAMIC(CDlgTabSource)

public:
	CDlgTabSource(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTabSource();

// Dialog Data
	enum { IDD = IDD_TAB_SOURCES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() { EditSource(); };
	virtual void	OnCancel() {};
public:
	
	static void		DrawComm( int item, int subItem, LPVOID param, CString& str );
	static void		DrawDescription( int item, int subItem, LPVOID param, CString& str );
	static void		DrawConnect( int item, int subItem, LPVOID param, CString& str );
	static void		DrawReceived( int item, int subItem, LPVOID param, CString& str );
	static void		DrawThreadTime( int item, int subItem, LPVOID param, CString& str );
	static void		DrawErrorCRC( int item, int subItem, LPVOID param, CString& str );

	CColorListCtrl	m_List;
	UINT					TimerID;

	virtual BOOL	OnInitDialog();
	afx_msg void	OnLvnGetdispinfoSourcesList(NMHDR *pNMHDR, LRESULT *pResult);
	
	BOOL					CreateSource();
	BOOL					EditSource();
	BOOL					DeleteSource();

	void					UpdateStatusBar();
	
	afx_msg void	OnNMDblclkSourcesList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMRClickSourcesList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSelectSource(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	
};


// CDlgSource dialog
// ======================================================================================

class CDlgSource : public CDialog
{
	DECLARE_DYNAMIC(CDlgSource)

public:
	CDlgSource(CSourceItem* pSource, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSource();

// Dialog Data
	enum { IDD = IDD_SOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();

public:
	CSourceItem*		m_pSource;

	CComboBox				m_Comm;
	CEdit						m_Description;
	
	virtual BOOL		OnInitDialog();

	BOOL						GetDlgData();
	void						SetDlgData();
};


// ======================================================================================