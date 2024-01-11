#pragma once
#include "afxcmn.h"
#include "ColorListCtrl.h"


// CDlgTabSocket dialog
// ======================================================================================

class CDlgTabSocket : public CDialog
{
	DECLARE_DYNAMIC(CDlgTabSocket)

public:
	CDlgTabSocket(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTabSocket();

// Dialog Data
	enum { IDD = IDD_TAB_SOCKETS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() {};
	virtual void	OnCancel() {};
public:
	
	static void		DrawIP( int item, int subItem, LPVOID param, CString& str );
	static void		DrawSentBytes( int item, int subItem, LPVOID param, CString& str );
	static void		DrawDescription( int item, int subItem, LPVOID param, CString& str );
	static void		DrawThreadTime( int item, int subItem, LPVOID param, CString& str );
	
	CColorListCtrl	m_List;
	UINT					TimerID;

	virtual BOOL	OnInitDialog();
	afx_msg void	OnLvnGetdispinfoSocketsList(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void	OnNMDblclkSocketsList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMRClickSocketsList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSelectSocket(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	
};

// ======================================================================================