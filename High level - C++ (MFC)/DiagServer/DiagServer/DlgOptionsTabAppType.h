#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgOptionsTabAppType dialog

class CDlgOptionsTabAppType : public CDialog
{
	DECLARE_DYNAMIC(CDlgOptionsTabAppType)

public:
	CDlgOptionsTabAppType(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOptionsTabAppType();

// Dialog Data
	enum { IDD = IDD_OPTIONS_TAB_APP_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void	OnOK() { };
	virtual void	OnCancel() {};
public:

	CButton						m_Server;
	CButton						m_Client;
	CIPAddressCtrl		m_ServerIP;
	CEdit							m_ServerPort;

	virtual BOOL			OnInitDialog();

	afx_msg void			OnServer();
	afx_msg void			OnClient();
	
};
