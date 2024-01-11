// CommThread.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "CommThread.h"
#include "crc8.h"

// COMM_STATE
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

CString	COMM_STATE::GetCommStateStr()
{
	if (nCommState < 0 || nCommState >= COMM_PORT_STATE_COUNT)
		return _T("Неисвестно");
	
	return CommStateStr[nCommState];
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CString	COMM_STATE::GetReceivedBytesStr()
{
	CString strReceivedBytes;

	strReceivedBytes.Format(!ReceivedBytesOk ? _T("Нет данных") : _T("%0.2f КБ"), 
													(double) ReceivedBytes / 1024);

	return strReceivedBytes;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CString	COMM_STATE::GetTimeLivingStr()
{
	CString	strTime;

	strTime.Format(_T("%02d/%02d/%02d   %02d:%02d:%02d"), 
									TimeLiving.wDay - 1, 
									TimeLiving.wMonth - 1, 
									TimeLiving.wYear - 1601, 
									TimeLiving.wHour, 
									TimeLiving.wMinute, 
									TimeLiving.wSecond); 

	return strTime;
}

// CCommThread
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CCommThread, CWinThread)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CCommThread::CCommThread(CSourceItem* pSource)
: pSource(pSource)
, pTimeoutThread(NULL)
, hComm(NULL)
, pReadMsgThread(NULL)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CCommThread::~CCommThread()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CCommThread::InitInstance()
{
	if (theApp.AppType == APP_TYPE_CLIENT)
		return TRUE;

	ExitTimeoutThread = FALSE;
	pTimeoutThread = AfxBeginThread(TimeoutThread, this);

	OpenPort();
	
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CCommThread::ExitInstance()
{
	ClosePort();

	ExitTimeoutThread = TRUE;
	if (pTimeoutThread)
	{
		WaitForSingleObject(pTimeoutThread->m_hThread, INFINITE);
		pTimeoutThread = NULL;
	}

	return CWinThread::ExitInstance();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CCommThread::OpenPort()
{
	if (InitComm())
	{
		CommState.nCommState = COMM_PORT_STATE_OK;
	
		ExitMsgThread = FALSE;
		pReadMsgThread = AfxBeginThread(ReadMsgThread, this);

		Sleep(100);
		RequestGetPortIO();
		Sleep(100);
		RequestGetMemoryParam();
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CCommThread::ClosePort()
{
	ExitMsgThread = TRUE;
	if (pReadMsgThread)
	{
		WaitForSingleObject(pReadMsgThread->m_hThread, INFINITE);
		pReadMsgThread = NULL;
	}

	if (hComm && hComm != INVALID_HANDLE_VALUE) 
	{
		PurgeComm(hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(hComm);
		hComm = NULL;
		CommState.nCommState = COMM_PORT_STATE_NOT_OPENED;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CCommThread::ReConnect()
{
	ClosePort();
	OpenPort();
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCommThread, CWinThread)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

UINT CCommThread::TimeoutThread(LPVOID pParam)
{
	CCommThread* pThis = (CCommThread*) pParam;
	if (!pThis || !pThis->pSource)
		return 0;

	int ReceiveBytes = 0;
	int Timeout = 0;
	while(!pThis->ExitTimeoutThread)
	{
		if (ReceiveBytes == pThis->CommState.ReceivedBytes)
		{	
			if(Timeout > TIMEOUT_SECOND)
				pThis->CommState.ReceivedBytesOk = FALSE;

			if(Timeout == TIMEOUT_COUNT)
			{
				pThis->ReConnect();
				Timeout = 0;
			}
			Timeout++;
		}
		else
			Timeout = 0;

		if (pThis->CommState.ErrorCRC > 5)
		{
			pThis->ReConnect();
			pThis->CommState.ErrorCRC = 0;
		}
		
		ReceiveBytes = pThis->CommState.ReceivedBytes;
		
		pThis->UpdateTimeLiving();

		//if (theApp.AppType == APP_TYPE_SERVER && theApp.pSocketThread)
			//theApp.SocketMsgQueue.Push(pThis->pSource->ID, CommState);
	
		Sleep(500);
	}



	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CCommThread::UpdateTimeLiving()
{
	if (!m_hThread)
		return;

	__int64 CreationTime, SystemTime, Time;
	
	GetThreadTimes(m_hThread, (FILETIME*) &CreationTime, 
														(FILETIME*) &Time, 
														(FILETIME*) &Time, 
														(FILETIME*) &Time);

	GetSystemTimeAsFileTime((FILETIME*)&SystemTime);
	SystemTime = SystemTime - CreationTime;
	FileTimeToSystemTime((FILETIME*) &SystemTime, &CommState.TimeLiving);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CCommThread::InitComm()
{
	if (!pSource)
		return FALSE;

	CString m_strComm = _T("\\\\.\\");
	m_strComm.Append(pSource->Comm);

	hComm = CreateFile(m_strComm, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hComm==INVALID_HANDLE_VALUE) 
	{
		//m_strErr = _T("Ошибка открытия последовательного порта");
		hComm = NULL;
		
		DWORD dw = GetLastError();

		return FALSE;
	}
	
	DCB dcb;
	if (!GetCommState(hComm, &dcb)) 
	{
		//m_strErr = _T("Ошибка получения состояния последовательного порта");
		CloseHandle(hComm);
		hComm = NULL;
		return FALSE;
	}
	
	dcb.BaudRate=CBR_9600;
	dcb.ByteSize=8;
	dcb.fParity=NOPARITY;
	dcb.StopBits=ONESTOPBIT;
		
	if (!SetCommState(hComm, &dcb)) 
	{
		//m_strErr = _T("Ошибка установки состояния последовательного порта");
		CloseHandle(hComm);
		hComm = NULL;
		return FALSE;
	}
	
	COMMTIMEOUTS ct;
	ct.ReadIntervalTimeout=5000;
	ct.ReadTotalTimeoutConstant=50000;
	ct.ReadTotalTimeoutMultiplier=0;
	ct.WriteTotalTimeoutConstant=0;
	ct.WriteTotalTimeoutMultiplier=0;

	if (!SetCommTimeouts(hComm, &ct)) 
	{
		//m_strErr = _T("Ошибка установки таймаутов для последовательного порта");
		CloseHandle(hComm);
		hComm = NULL;
		return FALSE;
	}
	
	if (!PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{ 
		//m_strErr = _T("Ошибка сброса значений буферов порта и регистров последовательного порта");
		CloseHandle(hComm);
		hComm = NULL;
		return FALSE;
	}
		
	DWORD CommEventMask = EV_RXCHAR;

	if (!SetCommMask(hComm, CommEventMask))
	{ 
		//m_strErr = _T("Ошибка установки маски для возникающих событий для последовательного порта");
		CloseHandle(hComm);
		hComm = NULL;
		return FALSE;
	}

	CommState.ReceivedBytes = 0;

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

UINT CCommThread::ReadMsgThread(LPVOID pParam)
{
	CCommThread* pThis = (CCommThread*) pParam;
	if (!pThis || !pThis->pSource)
		return 0;

	if (!pThis->hComm || pThis->hComm == INVALID_HANDLE_VALUE) 
		return 0;

	COMSTAT		CStat; 
	DWORD			dwEventMask			= 0;
	DWORD			dwErrorMask			= 0;
	DWORD			dwReadedBytes		= 0;

	BYTE			Msg[COMM_MSG_LEN];
	int				MsgLength = 0;

	while(!pThis->ExitMsgThread)
	{
		ClearCommError(pThis->hComm, &dwErrorMask, &CStat); 
		if (CStat.cbInQue > 0)
		{
			if(MsgLength >= COMM_MSG_LEN)
			{
				MsgLength = 0;
				continue;
			}

			ReadFile(pThis->hComm, &Msg[MsgLength], 1, &dwReadedBytes, NULL);
			pThis->CommState.ReceivedBytes += dwReadedBytes;
			
			pThis->CommState.ReceivedBytesOk = TRUE;

			if(Msg[MsgLength++] == 0x0D)
			{
				COMM_MSG CommMsg = *(COMM_MSG*)Msg;
				
				switch(CommMsg.Type)
				{
					case COMM_MSG_TYPE_STR: 

						theApp.SocketMsgQueue.Push(pThis->pSource->ID, CommMsg, MsgLength);

						break;

					case COMM_MSG_TYPE_DATA: 

						// Проверяем длину пришедшего пакета
						if (CommMsg.Header.DataHeader.Len != MsgLength - 5)
							continue;

						// Проверяем CRC8 пришедшего пакета
						if (CommMsg.Header.DataHeader.CRC8 != CalcCRC8(CommMsg.Header.DataHeader.Data, CommMsg.Header.DataHeader.Len))
						{
							pThis->CommState.ErrorCRC++;
							break;
						}

						theApp.SocketMsgQueue.Push(pThis->pSource->ID, CommMsg, MsgLength);

						break;
				}

				MsgLength = 0;
			}
		}
		Sleep(1);
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CCommThread::Send(BYTE* pMsg, int MsgLen)
{
	if (!hComm || hComm == INVALID_HANDLE_VALUE) 
		return 0;

	if (!pMsg || !MsgLen)
		return FALSE;

	if (MsgLen > COMM_MSG_LEN)
		MsgLen = COMM_MSG_LEN;

	DWORD dwWritenBytes = 0;
	WriteFile(hComm, pMsg, MsgLen, &dwWritenBytes, NULL);
	if(MsgLen != dwWritenBytes)
		return FALSE;

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CCommThread::RequestGetPortIO()
{
	COMM_REQUEST Request;
	
	Request.Type = COMM_REQUEST_GET_PORT_IO;
	Request.CRC8 = CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 
	
	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
}

void CCommThread::RequestSetDiscreteState(BYTE Port, BYTE Input, BYTE State)
{
	COMM_REQUEST Request;
	
	Request.Type		= COMM_REQUEST_SET_DISCRETE_STATE;
	Request.Data[0] = Port;
	Request.Data[1] = Input;
	Request.Data[2] = State;
	Request.CRC8		= CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 
		
	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
}

void CCommThread::RequestGetMemoryParam()
{
	COMM_REQUEST Request;
	
	Request.Type = COMM_REQUEST_GET_EEPROM_PARAM;
	Request.CRC8 = CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 
	
	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
}

void CCommThread::RequestSetMemoryParam(BYTE Address, BYTE aByte)
{
	COMM_REQUEST Request;
	
	Request.Type		= COMM_REQUEST_SET_EEPROM_PARAM;
	Request.Data[0]	= Address;
	Request.Data[1]	= aByte;
	Request.CRC8		= CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 

	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

