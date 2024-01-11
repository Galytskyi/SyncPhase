#pragma once
#include "afxcmn.h"
#include "ColorListCtrl.h"

// CDlgTabMessage dialog

class CDlgTabMessage : public CDialog
{
	DECLARE_DYNAMIC(CDlgTabMessage)

public:
	CDlgTabMessage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTabMessage();

// Dialog Data
	enum { IDD = IDD_TAB_MESSAGES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() {};
	virtual void	OnCancel() {};
public:
	
	static void		DrawNo( int item, int subItem, LPVOID param, CString& str );
	static void		DrawTime( int item, int subItem, LPVOID param, CString& str );
	static void		DrawDate( int item, int subItem, LPVOID param, CString& str );
	static void		DrawMessage( int item, int subItem, LPVOID param, CString& str );

	CColorListCtrl	m_List;
	UINT					TimerID;

	virtual BOOL	OnInitDialog();
	afx_msg void	OnLvnGetdispinfoMessagesList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void	OnNMRClickMessagesList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
};
