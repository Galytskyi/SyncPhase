
// DiagServer.cpp : ���������� ��������� ������� ��� ����������.
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgDiagServer.h"

#include "DataFile.h"
#include "..\..\INCLUDE\RegAdmin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDiagServerApp
// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDiagServerApp, CWinAppEx)
	ON_COMMAND(ID_HELP,												&CWinApp::OnHelp)
END_MESSAGE_MAP()

// �������� CDiagServerApp
// -------------------------------------------------------------------------------------------------------------------------------------------------

CDiagServerApp::CDiagServerApp()
:	AppType(APP_TYPE_SERVER)
,	ServerIP(0x7F000001)
,	ServerPort(SERVER_PORT)
, pSocketThread(NULL)
{
	// TODO: �������� ��� ��������,
	// ��������� ���� ������ ��� ������������� � InitInstance
}

// ������������ ������ CDiagServerApp
// -------------------------------------------------------------------------------------------------------------------------------------------------

CDiagServerApp theApp;

// ������������� CDiagServerApp
// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDiagServerApp::InitInstance()
{
	// InitCommonControlsEx() ��������� ��� Windows XP, ���� ��������
	// ���������� ���������� ComCtl32.dll ������ 6 ��� ����� ������� ������ ��� ���������
	// ������ �����������. � ��������� ������ ����� ��������� ���� ��� �������� ������ ����.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �������� ���� �������� ��� ��������� ���� ����� ������� ����������, ������� ���������� ������������
	// � ����� ����������.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ����������� �������������
	// ���� ��� ����������� �� ������������ � ���������� ��������� ������
	// ��������� ������������ �����, ���������� ������� �� ���������
	// ���������� �������� �������������, ������� �� ���������
	// �������� ������ �������, � ������� �������� ���������
	// TODO: ������� �������� ��� ������ �� ���-������ ����������,
	// �������� �� �������� �����������
	SetRegistryKey(_T("AVR"));

	
	if (!InitializeSocket())
	{
		AfxMessageBox(_T("������ ������������� Sockets 2.2!"));
		return FALSE;
	}

	LoadParam();

	GetCurrentDirectory(MAX_PATH, (LPTSTR) CurrentDirectoryStr);
	
	InitDatabase();
	SourceBase.Load();

	pSocketThread = new CSocketThread(AppType, ServerIP);
	if (pSocketThread)
		pSocketThread->InitInstance();
	
	CDlgDiagServer dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ������� ��� ��� ��������� �������� ����������� ����
		//  � ������� ������ "��"
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ������� ��� ��� ��������� �������� ����������� ����
		//  � ������� ������ "������"
	}

	// ��������� ���������� ���� �������, ���������� �������� FALSE, ����� ����� ���� ����� ��
	//  ���������� ������ ������� ���������� ��������� ����������.
	return FALSE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CDiagServerApp::ExitInstance()
{
	ClientSocketBase.RemoveAll();
	
	if (pSocketThread)
	{
		pSocketThread->ExitInstance();
		delete pSocketThread;
		pSocketThread = NULL;
	}

	WSACleanup();

	SourceBase.Save();
	SourceBase.RemoveAll();
	
	//SaveParam();

	return CWinAppEx::ExitInstance();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDiagServerApp::InitDatabase()
{
  TCHAR DataDir[MAX_PATH];

  _tcscpy_s(DataDir, theApp.CurrentDirectoryStr);
	_tcscat_s(DataDir, POSTFIX_DATAFILE);

	if (!PathFileExists(DataDir))
		CreateDirectory(DataDir, NULL); 

	CString strAttributes;

	strAttributes = _T("DSN=AVR");							
	strAttributes.AppendChar('\0');	
	strAttributes.Append(_T("Description=AVR") );
	strAttributes.AppendChar('\0');	
	strAttributes.Append(_T("DefaultDir="));
	strAttributes.Append(DataDir);
	strAttributes.AppendChar('\0');	

	if (IsAdmin())
		if (!SQLConfigDataSource(NULL, ODBC_ADD_SYS_DSN, _T("Driver do Microsoft dBase (*.dbf)"), strAttributes))
			AfxMessageBox(_T("������ ����������� ���� ������"));
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDiagServerApp::LoadParam()
{
	AppType			= GetProfileInt(_T("Settings\\AppType"), _T("AppType"),			APP_TYPE_SERVER);
	ServerIP		= GetProfileInt(_T("Settings\\AppType"), _T("ServerIP"),		0x7F000001);
	ServerPort	= GetProfileInt(_T("Settings\\AppType"), _T("ServerPort"),	SERVER_PORT);
}
	
// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDiagServerApp::SaveParam()
{
	WriteProfileInt(_T("Settings\\AppType"), _T("AppType"),			AppType);
	WriteProfileInt(_T("Settings\\AppType"), _T("ServerIP"),		ServerIP);
	WriteProfileInt(_T("Settings\\AppType"), _T("ServerPort"),	ServerPort);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------