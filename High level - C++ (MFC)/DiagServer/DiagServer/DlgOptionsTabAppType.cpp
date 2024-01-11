// DlgOptionsTabAppType.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgOptionsTabAppType.h"


// CDlgOptionsTabAppType dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgOptionsTabAppType, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgOptionsTabAppType::CDlgOptionsTabAppType(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptionsTabAppType::IDD, pParent)
{

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgOptionsTabAppType::~CDlgOptionsTabAppType()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgOptionsTabAppType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,		ID_OTAT_SERVER,				m_Server);
	DDX_Control(pDX,		ID_OTAT_CLIENT,				m_Client);
	DDX_Control(pDX,		ID_OTAT_SERVER_IP,		m_ServerIP);
	DDX_Control(pDX,		ID_OTAT_SERVER_PORT,	m_ServerPort);

	DDX_IPAddress( pDX, ID_OTAT_SERVER_IP,		theApp.ServerIP);
	DDX_Text( pDX,			ID_OTAT_SERVER_PORT,	theApp.ServerPort);

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgOptionsTabAppType, CDialog)
	ON_BN_CLICKED(ID_OTAT_SERVER, &CDlgOptionsTabAppType::OnServer)
	ON_BN_CLICKED(ID_OTAT_CLIENT, &CDlgOptionsTabAppType::OnClient)


END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgOptionsTabAppType::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	switch(theApp.AppType)
	{
		case APP_TYPE_SERVER: m_Server.SetCheck(TRUE); OnServer(); break;
		case APP_TYPE_CLIENT: m_Client.SetCheck(TRUE); OnClient(); break;
	}


	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------


void CDlgOptionsTabAppType::OnServer()
{
	m_ServerIP.EnableWindow(FALSE);
	m_ServerPort.EnableWindow(FALSE);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------


void CDlgOptionsTabAppType::OnClient()
{
	m_ServerIP.EnableWindow(TRUE);
	m_ServerPort.EnableWindow(TRUE);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
