
// DiagServer.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы
#include "SourceItem.h"
#include "SocketThread.h"

// ======================================================================================

const int APP_TYPE_SERVER = 0,
					APP_TYPE_CLIENT = 1;

const int APP_TYPE_COUNT	= 2;

const TCHAR* const AppTypeStr[APP_TYPE_COUNT] = 
{
	_T("Сервер"),
	_T("Клиент"),
};

const int SERVER_PORT = 3000;

// ======================================================================================

// CDiagServerApp:
// О реализации данного класса см. DiagServer.cpp
//

class CDiagServerApp : public CWinAppEx
{
public:
	CDiagServerApp();

// Переопределение
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

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CDiagServerApp theApp;