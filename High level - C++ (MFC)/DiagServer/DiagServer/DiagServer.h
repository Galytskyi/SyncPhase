
// DiagServer.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������
#include "SourceItem.h"
#include "SocketThread.h"

// ======================================================================================

const int APP_TYPE_SERVER = 0,
					APP_TYPE_CLIENT = 1;

const int APP_TYPE_COUNT	= 2;

const TCHAR* const AppTypeStr[APP_TYPE_COUNT] = 
{
	_T("������"),
	_T("������"),
};

const int SERVER_PORT = 3000;

// ======================================================================================

// CDiagServerApp:
// � ���������� ������� ������ ��. DiagServer.cpp
//

class CDiagServerApp : public CWinAppEx
{
public:
	CDiagServerApp();

// ���������������
public:
	TCHAR								CurrentDirectoryStr[MAX_PATH];

	int									AppType;
	DWORD								ServerIP;
	DWORD								ServerPort;

	void								InitDatabase();
	CSourceBase					SourceBase;
	CSocketMsgQueue			SocketMsgQueue;
	CSocketThread*			pSocketThread;
	CClientSocketBase		ClientSocketBase;

	
	void								LoadParam();
	void								SaveParam();

	virtual BOOL				InitInstance();
	virtual int					ExitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CDiagServerApp theApp;