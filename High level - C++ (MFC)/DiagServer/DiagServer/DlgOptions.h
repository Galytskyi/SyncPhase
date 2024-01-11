#pragma once


// ======================================================================================

struct OPTIONS_TAB_HEADER
{
	TCHAR*					Description;
	int							nID;
};

const int OPTIONS_TAB_APP_TYPE		= 0;

const int OPTIONS_TAB_COUNT				= 1; 

const OPTIONS_TAB_HEADER OptionsTabHeader[OPTIONS_TAB_COUNT] =
{
	{_T("Тип приложения"),		IDD_OPTIONS_TAB_APP_TYPE},
};


// CDlgOptions dialog
// ======================================================================================

class CDlgOptions : public CDialog
{
	DECLARE_DYNAMIC(CDlgOptions)

public:
	CDlgOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOptions();

// Dialog Data
	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
private:
	CTabCtrl						m_TabCtrl;
	
public:

	CArray<CDialog*>		m_TabArray;

	virtual BOOL				OnInitDialog();

	void								CreateTabs();
	void								RemoveTabs();
	afx_msg void				OnSelectTab(NMHDR *pNMHDR, LRESULT *pResult);
	void								SelectTab(int nTab);

};

// ======================================================================================