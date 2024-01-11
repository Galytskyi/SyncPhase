// DlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgOptions.h"

#include "DlgOptionsTabAppType.h"

// CDlgOptions dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgOptions, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

static CRect rDlgOptionsRect;

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgOptions::CDlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptions::IDD, pParent)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgOptions::~CDlgOptions()
{
	RemoveTabs();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTIONS_TAB, m_TabCtrl);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgOptions, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_OPTIONS_TAB,					&CDlgOptions::OnSelectTab)
END_MESSAGE_MAP()


// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_TabCtrl.GetWindowRect(&rDlgOptionsRect);
	ScreenToClient(&rDlgOptionsRect);
		
	CreateTabs();

	return TRUE; 
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgOptions::CreateTabs()
{
	CDialog* pDlg	=	NULL;
	for(int t = 0; t < OPTIONS_TAB_COUNT; t++)
	{
		m_TabCtrl.InsertItem( t, OptionsTabHeader[t].Description );
				
		switch(t)
		{
			case OPTIONS_TAB_APP_TYPE:			pDlg = new CDlgOptionsTabAppType;				break;
		}
		if (pDlg)
		{
			pDlg->Create( OptionsTabHeader[t].nID, this );
			m_TabArray.Add(pDlg);
		}
	}

	SelectTab(0);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgOptions::RemoveTabs()
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

void CDlgOptions::OnSelectTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nTab = m_TabCtrl.GetCurSel();
	if (nTab == CB_ERR)
		return;

	SelectTab(nTab);
	
	*pResult = 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgOptions::SelectTab(int nTab)
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
											rDlgOptionsRect.left + 3, rDlgOptionsRect.top + 22, 
											rDlgOptionsRect.right-rDlgOptionsRect.left - 8, rDlgOptionsRect.bottom-rDlgOptionsRect.top - 25, 
											SWP_NOZORDER);
	pDlg->ShowWindow(SW_SHOW);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgOptions::OnOK()
{
	CDlgOptionsTabAppType* pDlgAppType = (CDlgOptionsTabAppType*) m_TabArray[OPTIONS_TAB_APP_TYPE];
	if (pDlgAppType)
	{	
		int		AppType;
		DWORD	ServerIP;
		DWORD	ServerPort;

		if (pDlgAppType->m_Server.GetCheck()) AppType = APP_TYPE_SERVER;
		if (pDlgAppType->m_Client.GetCheck())	AppType = APP_TYPE_CLIENT;

		CString strPort;
		pDlgAppType->m_ServerIP.GetAddress(ServerIP);
		pDlgAppType->m_ServerPort.GetWindowText(strPort);
		ServerPort = StrToInt(strPort);

		theApp.WriteProfileInt(_T("Settings\\AppType"), _T("AppType"),		AppType);
		theApp.WriteProfileInt(_T("Settings\\AppType"), _T("ServerIP"),		ServerIP);
		theApp.WriteProfileInt(_T("Settings\\AppType"), _T("ServerPort"),	ServerPort);
	}

	AfxMessageBox(_T("Изменения вступят в силу только после перзапуска приложения!"));

	CDialog::OnOK();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------