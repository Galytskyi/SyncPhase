// SocketThread.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "SocketThread.h"
#include "DataFile.h"
#include "crc8.h"

#include "..\..\INCLUDE\util.h"

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL InitializeSocket() 
{
	WSADATA	wsd;
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return FALSE;

	if ( LOBYTE( wsd.wVersion ) != 2 || HIBYTE( wsd.wVersion ) != 2 ) 
	{
    WSACleanup();
    return FALSE; 
	}
	
	return TRUE;
}


// CSocketMsgQueue
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

CSocketMsgQueue::CSocketMsgQueue()
{
	ExitRemoveMsgThread = FALSE;
	pRemoveMsgThread = AfxBeginThread(RemoveMsgThread, this);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSocketMsgQueue::~CSocketMsgQueue()
{
	if (pRemoveMsgThread)
	{
		ExitRemoveMsgThread = TRUE;
		WaitForSingleObject(pRemoveMsgThread->m_hThread, INFINITE);
		pRemoveMsgThread = NULL;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CSocketMsgQueue::GetCount()
{
	int nItemCount = 0;

	_critSect.Lock();
		nItemCount = (int) SocketMsgList.GetCount();
	_critSect.Unlock();

	return nItemCount;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSocketMsgQueue::Push(SOCKET_MSG Msg)
{
	GetLocalTime(&Msg.TimeStamp);
	
	_critSect.Lock();
		SocketMsgList.AddTail(Msg);
	_critSect.Unlock();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSocketMsgQueue::Push(int SourceID, COMM_MSG CommMsg, int CommMsgLen)
{
	_critSect.Lock();

		SOCKET_MSG SocketMsg;

		SocketMsg.MsgLen			= sizeof(SOCKET_MSG);
	
		SocketMsg.SourceID		= SourceID;
		GetLocalTime(&SocketMsg.TimeStamp);
	
		SocketMsg.CommMsgLen	= CommMsgLen;
		SocketMsg.CommMsg			= CommMsg;

		SocketMsg.CRC8				= CalcCRC8((BYTE*) &CommMsg, sizeof(COMM_MSG));
		
		SocketMsgList.AddTail(SocketMsg);

	_critSect.Unlock();
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

SOCKET_MSG CSocketMsgQueue::Pop()
{
	SOCKET_MSG Msg;

	_critSect.Lock();
		Msg = SocketMsgList.RemoveHead();
	_critSect.Unlock();

	return Msg;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

UINT CSocketMsgQueue::RemoveMsgThread(LPVOID pParam)
{
	CSocketMsgQueue* pThis = (CSocketMsgQueue*) pParam;
	if (!pThis)
		return 0;

	CString TimeStr;

	while(!pThis->ExitRemoveMsgThread)
	{
		if (pThis->GetCount())
		{
			SOCKET_MSG SocketMsg = pThis->Pop();

			pThis->ProcessingSocketMsg(SocketMsg);
		}

		Sleep(1);
	}
	
	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketMsgQueue::ProcessingSocketMsg(SOCKET_MSG SocketMsg)
{
	if (SocketMsg.MsgLen != sizeof(SOCKET_MSG)) 
		return FALSE;
	
	BYTE CRC8 =  CalcCRC8((BYTE*) &SocketMsg.CommMsg, sizeof(COMM_MSG));
	if (SocketMsg.CRC8 != CalcCRC8((BYTE*) &SocketMsg.CommMsg, sizeof(COMM_MSG)))
		return FALSE;

	CSourceItem* pSource = theApp.SourceBase.GetAt(SocketMsg.SourceID);
	if (!pSource)
		return FALSE;
	
	if(theApp.AppType == APP_TYPE_SERVER && theApp.pSocketThread)
		theApp.pSocketThread->Send((BYTE*) &SocketMsg, sizeof(SOCKET_MSG));

	CString				TimeStr;
	DIAG_MSG_STR	DiagMsgStr;
	CDataFile			MsgTable;

	switch(SocketMsg.CommMsg.Type)
	{
		case COMM_MSG_TYPE_STR:

			DiagMsgStr.SourceID = SocketMsg.SourceID;

			TimeStr.Format(	_T("%02d:%02d:%02d.%03d"), 
											SocketMsg.TimeStamp.wHour, 
											SocketMsg.TimeStamp.wMinute, 
											SocketMsg.TimeStamp.wSecond, 
											SocketMsg.TimeStamp.wMilliseconds);           
			SafeStrncpy(DiagMsgStr.TimeStr, TimeStr, DIAG_MSG_TIME_LEN );	
			
			TimeStr.Format(	_T("%02d/%02d/%04d"), SocketMsg.TimeStamp.wDay,  SocketMsg.TimeStamp.wMonth, SocketMsg.TimeStamp.wYear);           
			SafeStrncpy(DiagMsgStr.DateStr, TimeStr, DIAG_MSG_DATE_LEN );	
			
			DiagMsgStr.dwTimeCode.SetTime(SocketMsg.TimeStamp);
			DiagMsgStr.dwTimeCode.SetDate(SocketMsg.TimeStamp);

			DiagMsgStr.StrType = SocketMsg.CommMsg.Header.StrHeader.StrType;
			
			int MsgStrLen;
			MsgStrLen = SocketMsg.CommMsgLen >= DIAG_MSG_STR_LEN ? DIAG_MSG_STR_LEN : SocketMsg.CommMsgLen - 2;
			SocketMsg.CommMsg.Header.StrHeader.Str[MsgStrLen] = 0; 
			SafeStrncpy( DiagMsgStr.MsgStr,	CA2T(SocketMsg.CommMsg.Header.StrHeader.Str), DIAG_MSG_STR_LEN );	

			_critSect.Lock();

				if (MsgTable.Open(TABLE_TYPE_DIAG_MSG_STR))
				{
					MsgTable.WriteRecord(&DiagMsgStr);
					MsgTable.Close();
				}

				pSource->MsgStrArray.Add(DiagMsgStr);

			_critSect.Unlock();

			break;

		case COMM_MSG_TYPE_DATA:

			switch(SocketMsg.CommMsg.Header.DataHeader.DataType)
			{
				case COMM_MSG_TYPE_DATA_PORT_IO: 
					memcpy(pSource->PortIO, SocketMsg.CommMsg.Header.DataHeader.Data, DIAG_DISCRETE_ARRAY_LEN); 
					break;

				case COMM_MSG_TYPE_DATA_PORT_STATE: 
					memcpy(pSource->DiscretePortState, SocketMsg.CommMsg.Header.DataHeader.Data, DIAG_DISCRETE_ARRAY_LEN); 
					memcpy(pSource->AnalogInputState, &SocketMsg.CommMsg.Header.DataHeader.Data[DIAG_DISCRETE_ARRAY_LEN], DIAG_ANALOG_ARRAY_LEN); 
					break;

				case COMM_MSG_TYPE_DATA_EEPROM:			
					memcpy(pSource->MemoryByteState, SocketMsg.CommMsg.Header.DataHeader.Data, EEPROM_BYTE_COUNT * sizeof(BYTE)); 
					break;
			}

			break;
	}

	return TRUE;
}

// CSocketThread
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CSocketThread, CWinThread)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSocketThread::CSocketThread(int Type, DWORD ip, SOCKET socket, CSocketThread* parent)
: SocketType(Type)
, IP(ip)
, hSocket(socket)
, pParent(parent)
, hSocketEvent(NULL) 
, hCloseSocket(NULL) 
, bConnect(FALSE)
, SentBytes(0)
, ExitEventThread(FALSE)
, pEventThread(NULL)
, ExitConnectThread(FALSE)
, pConnectThread(NULL)
, ExitWaitCloseSokcetThread(FALSE)
, pWaitCloseSocketThread(NULL)
{
	hSocketEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hCloseSocket = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD DescriptionLen = SOCKET_DESCRIPTION_LEN;
	GetComputerName(Description, &DescriptionLen); 
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSocketThread::~CSocketThread()
{
	if (hSocketEvent)
		CloseHandle(hSocketEvent);

	if (hCloseSocket)
		CloseHandle(hCloseSocket);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::InitInstance()
{
	if(CreateSocket())
	{
		ExitWaitCloseSokcetThread = FALSE;
		pWaitCloseSocketThread = AfxBeginThread(WaitCloseSocketThread, this) ;
	}
	else
		AfxMessageBox(_T("Ошибка создания сокета!"));
	
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CSocketThread::ExitInstance()
{
	CloseSocket();

	ExitWaitCloseSokcetThread = TRUE;
	if (pWaitCloseSocketThread)
	{
		WaitForSingleObject(pWaitCloseSocketThread->m_hThread, INFINITE);
		pWaitCloseSocketThread = NULL;
	}

	return CWinThread::ExitInstance();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CSocketThread, CWinThread)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::CreateSocket()
{
	BOOL Result = FALSE;

	switch(SocketType)
	{
		case APP_TYPE_SERVER: Result = CreateServerSocket(); break;
		case APP_TYPE_CLIENT: Result = CreateClientSocket(); break;
	}

	return Result;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::CreateServerSocket()
{
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sockaddr_in));

	// Создание сокета
	hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (hSocket == INVALID_SOCKET) 
		return FALSE;

	// Заполнение структуры sockaddr_in
	sa.sin_family				= AF_INET;
	sa.sin_addr.s_addr	= htonl(INADDR_ANY);
	sa.sin_port					= htons((u_short)theApp.ServerPort);
	
	// Связывание адреса с переменной sockaddr_in
	if(bind(hSocket, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR)
		return FALSE;

	// События на присоедение клиента, и закрытие
	if(WSAEventSelect(hSocket, hSocketEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		return FALSE;

	// Запуск прослушивания порта
	if(listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
		return FALSE;

	ExitEventThread = FALSE;
	pEventThread = AfxBeginThread(EventThread, this) ;
	
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::CreateClientSocket()
{
	// Создание сокета
	if (hSocket == INVALID_SOCKET) 
		hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	if (hSocket == INVALID_SOCKET) 
		return FALSE;

	// События на присоедение клиента, и закрытие
	if(WSAEventSelect(hSocket, hSocketEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		return FALSE;

	ExitEventThread = FALSE;
	pEventThread = AfxBeginThread(EventThread, this) ;

	if (theApp.AppType == APP_TYPE_CLIENT)
	{
		ExitConnectThread = FALSE;
		pConnectThread = AfxBeginThread(ConnectThread, this) ;
	}

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSocketThread::CloseSocket()
{
	ExitEventThread = TRUE;
	if (pEventThread)
	{
		WaitForSingleObject(pEventThread->m_hThread, INFINITE);
		pEventThread = NULL;
	}

	ExitConnectThread = TRUE;
	if (pConnectThread)
	{
		WaitForSingleObject(pConnectThread->m_hThread, INFINITE);
		pConnectThread = NULL;
	}
	
	if (hSocket != INVALID_SOCKET)
	{
		closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::Accept()
{
	sockaddr_in sa;
	INT addrLen;
	addrLen = sizeof(sa);

	SOCKET hClientSocket = WSAAccept(hSocket, (sockaddr*) &sa, &addrLen, NULL, NULL);
	if (hSocket == INVALID_SOCKET) 
		return FALSE;

	CSocketThread* pSocketThread = new CSocketThread(APP_TYPE_CLIENT, sa.sin_addr.S_un.S_addr, hClientSocket, this);
	if (!pSocketThread)
		return FALSE;

	pSocketThread->CreateThread();
	
	theApp.ClientSocketBase.Add(pSocketThread);
		
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::Connect()
{
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sockaddr_in));

	// Заполнение структуры с адресом сервера и номером порта
	sa.sin_family				= AF_INET;
	sa.sin_addr.s_addr	= htonl(IP);
	sa.sin_port					= htons( (u_short) theApp.ServerPort);
	
	if(WSAConnect(hSocket, (sockaddr*) &sa, sizeof(sa), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

UINT CSocketThread::EventThread(LPVOID pParam)
{
	CSocketThread* pThis = (CSocketThread*) pParam;
	if (!pThis)
		return 0;

	int ExitCode = -1;

	while(!pThis->ExitEventThread)
	{
		DWORD IsEvent = WaitForSingleObject(pThis->hSocketEvent, 1);
		if (IsEvent == WAIT_TIMEOUT || IsEvent == WAIT_FAILED)
			continue;

		WSANETWORKEVENTS wsaEvents;

		if(WSAEnumNetworkEvents(pThis->hSocket, pThis->hSocketEvent, &wsaEvents) ==  SOCKET_ERROR)
		{
			int WSAErrorCode = WSAGetLastError();
			ResetEvent(pThis->hSocketEvent);
			continue;
		}

		ResetEvent(pThis->hSocketEvent);

		switch(pThis->SocketType)
		{
			case APP_TYPE_SERVER:
				if (wsaEvents.lNetworkEvents & FD_ACCEPT) 
					pThis->Accept();
				break;

			case APP_TYPE_CLIENT:
				if (wsaEvents.lNetworkEvents & FD_CLOSE) 
				{
					pThis->bConnect = FALSE;
					SetEvent(pThis->hCloseSocket);
				}

				if (wsaEvents.lNetworkEvents & FD_READ) 
				{
					pThis->bConnect = TRUE;
					pThis->Recv();
				}

				if (wsaEvents.lNetworkEvents & FD_WRITE) 
					pThis->bConnect = TRUE;

				break;
		}
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

UINT CSocketThread::ConnectThread(LPVOID pParam)
{
	CSocketThread* pThis = (CSocketThread*) pParam;
	if (!pThis)
		return 0;

	while(!pThis->ExitConnectThread)
	{
		if (pThis->bConnect)
			continue;
		
		pThis->bConnect = pThis->Connect();
				

		Sleep(100);
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

UINT CSocketThread::WaitCloseSocketThread(LPVOID pParam)
{
	CSocketThread* pThis = (CSocketThread*) pParam;
	if (!pThis)
		return 0;

	DWORD IsEvent;

	while(!pThis->ExitWaitCloseSokcetThread)
	{
		pThis->UpdateTimeLiving();

		switch(theApp.AppType)
		{
			case APP_TYPE_SERVER: 

				if (pThis->SocketType == APP_TYPE_CLIENT)
					continue;

				IsEvent = WaitForMultipleObjects(theApp.ClientSocketBase.GetCount(), theApp.ClientSocketBase.CloseEventArray, FALSE, 10);
				if (IsEvent == WAIT_TIMEOUT || IsEvent == WAIT_FAILED )
					continue;

				theApp.ClientSocketBase.Remove(IsEvent);
								
				break;
		
			case APP_TYPE_CLIENT:

				IsEvent = WaitForSingleObject(pThis->hCloseSocket, 10);
				if (IsEvent != WAIT_OBJECT_0)
					continue;

				pThis->CloseSocket();
				pThis->CreateSocket();
				break;
		}
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::Recv()
{
	SOCKET_MSG			SocketMsg;
	SOCKET_REQUEST	SocketReuest;

	DWORD dwRecvBytes = 0;
	DWORD dwRecvFlags = 0;


	WSABUF WSABuffer;

	if (theApp.AppType == APP_TYPE_SERVER)
	{
		WSABuffer.buf = (CHAR*) &SocketReuest;
		WSABuffer.len = sizeof(SOCKET_REQUEST);
	
		if(WSARecv(hSocket, &WSABuffer, 1, &dwRecvBytes, &dwRecvFlags, 0, NULL) == SOCKET_ERROR)
			return FALSE;

		if (dwRecvBytes != sizeof(SOCKET_REQUEST))
			return FALSE;

		RecvBytes += dwRecvBytes;

		if (SocketReuest.MsgLen != sizeof(SOCKET_REQUEST))
			return FALSE;

		CSourceItem* pSource = theApp.SourceBase.GetAt(SocketReuest.SourceID);
		if(!pSource || !pSource->pCommThread)
			return FALSE;

		switch(SocketReuest.CommRequest.Type)
		{
			case COMM_REQUEST_GET_PORT_IO:					
				pSource->pCommThread->RequestGetPortIO();											
				break;
			case COMM_REQUEST_SET_DISCRETE_STATE:		
				pSource->pCommThread->RequestSetDiscreteState(SocketReuest.CommRequest.Data[0], 
																											SocketReuest.CommRequest.Data[1], 
																											SocketReuest.CommRequest.Data[2]);	
				break;
			case COMM_REQUEST_GET_EEPROM_PARAM:		
				pSource->pCommThread->RequestGetMemoryParam();						
				break;
			case COMM_REQUEST_SET_EEPROM_PARAM:		
				pSource->pCommThread->RequestSetMemoryParam(	SocketReuest.CommRequest.Data[0], 
																											SocketReuest.CommRequest.Data[1]);						
				break;
		}
	}

	if (theApp.AppType == APP_TYPE_CLIENT)
	{
		WSABuffer.buf = (CHAR*) &SocketMsg;
		WSABuffer.len = sizeof(SOCKET_MSG);
	
		if(WSARecv(hSocket, &WSABuffer, 1, &dwRecvBytes, &dwRecvFlags, 0, NULL) == SOCKET_ERROR)
			return FALSE;

		if (dwRecvBytes != sizeof(SOCKET_MSG))
			return FALSE;

		RecvBytes += dwRecvBytes;

		theApp.SocketMsgQueue.Push(SocketMsg);
	}

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSocketThread::Send(BYTE* pBuffer, int Len)
{
	if (!pBuffer || !Len)
		return FALSE;
	
	WSABUF WSABuffer;

	WSABuffer.buf = (CHAR*) pBuffer;
	WSABuffer.len = Len;

	DWORD dwSentBytes = 0;

	if (theApp.AppType == APP_TYPE_SERVER)
	{
		int nSocketCount = theApp.ClientSocketBase.GetCount();
		for (int s = 0; s < nSocketCount; s++)
		{
			CSocketThread* pSocket = theApp.ClientSocketBase.GetAt(s);
			if (!pSocket)
				continue;

			if(WSASend(pSocket->GetSocketHandle(), &WSABuffer, 1, &dwSentBytes, 0, NULL, NULL) != SOCKET_ERROR)
				pSocket->SentBytes += dwSentBytes;
		}
	}
	
	if (theApp.AppType == APP_TYPE_CLIENT)
	{
		if(WSASend(hSocket, &WSABuffer, 1, &dwSentBytes, 0, NULL, NULL) != SOCKET_ERROR)
			SentBytes += dwSentBytes;
	}

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CString	CSocketThread::GetSentBytesStr()
{
	CString strReceivedBytes;

	strReceivedBytes.Format(_T("%0.2f КБ"), (double) SentBytes / 1024);

	return strReceivedBytes;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSocketThread::UpdateTimeLiving()
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
	FileTimeToSystemTime((FILETIME*) &SystemTime, &TimeLiving);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CString	CSocketThread::GetTimeLivingStr()
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


// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSocketThread::RequestGetPortIO(int SourceID)
{
	SOCKET_REQUEST	SocketRequest;
	COMM_REQUEST		CommRequest;
	
	CommRequest.Type = COMM_REQUEST_GET_PORT_IO;
	CommRequest.CRC8 = CalcCRC8(CommRequest.Data, COMM_REQUEST_DATA_LEN); 
	
	SocketRequest.MsgLen				= sizeof(SOCKET_REQUEST);
	SocketRequest.SourceID			= SourceID;
	SocketRequest.CommRequest		= CommRequest;

	Send((BYTE*) &SocketRequest, sizeof(SOCKET_REQUEST));
}

//void CCommThread::RequestSetDiscreteState(BYTE Port, BYTE Input, BYTE State)
//{
//	COMM_REQUEST Request;
//	
//	Request.Type		= COMM_REQUEST_SET_DISCRETE_STATE;
//	Request.Data[0] = Port;
//	Request.Data[1] = Input;
//	Request.Data[2] = State;
//	Request.CRC8		= CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 
//		
//	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
//}
//
//void CCommThread::RequestGetMemoryParam()
//{
//	COMM_REQUEST Request;
//	
//	Request.Type = COMM_REQUEST_GET_EEPROM_PARAM;
//	Request.CRC8 = CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 
//	
//	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
//}
//
//void CCommThread::RequestSetMemoryParam(BYTE Address, BYTE aByte)
//{
//	COMM_REQUEST Request;
//	
//	Request.Type		= COMM_REQUEST_SET_EEPROM_PARAM;
//	Request.Data[0]	= Address;
//	Request.Data[1]	= aByte;
//	Request.CRC8		= CalcCRC8(Request.Data, COMM_REQUEST_DATA_LEN); 
//
//	Send((BYTE*)&Request, sizeof(COMM_REQUEST));
//}


// CClientSocketBase
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

CClientSocketBase::CClientSocketBase()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CClientSocketBase::~CClientSocketBase()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CClientSocketBase::GetCount()
{
	int nItemCount = 0;

	_critSect.Lock();
		nItemCount = (int) SocketArray.GetCount();
	_critSect.Unlock();

	return nItemCount;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CClientSocketBase::Add(CSocketThread* pSocket)
{
	if(!pSocket)
		return -1;

	int nItem = -1;

	_critSect.Lock();
		nItem = SocketArray.Add(pSocket);
	_critSect.Unlock();

	UpdateCloseEventArray();

	return nItem;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CClientSocketBase::Remove(int nItem)
{
	if (nItem < 0 || nItem >= GetCount())
		return FALSE;
	
	_critSect.Lock();

		CSocketThread* pSocket = SocketArray[nItem];
		if (pSocket)
		{
			pSocket->ExitInstance();
			delete pSocket;
			SocketArray.RemoveAt(nItem);
		}

	_critSect.Unlock();

	UpdateCloseEventArray();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CClientSocketBase::Remove(CSocketThread* pSocket)
{
	if (!pSocket)
		return FALSE;

	_critSect.Lock();

		int nSocketCount = (int) SocketArray.GetCount();
		for(int s = 0; s < nSocketCount ; s++)
		{
			CSocketThread* pNSocket = SocketArray[s];
			if (!pNSocket)
				continue;

			if (pNSocket == pSocket)
			{
				pSocket->ExitInstance();
				delete pSocket;
				SocketArray.RemoveAt(s);
				break;
			}
		}

	_critSect.Unlock();

	UpdateCloseEventArray();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CClientSocketBase::RemoveAll()
{
	_critSect.Lock();

		int nSocketCount = (int) SocketArray.GetCount();
		for(int s = nSocketCount - 1; s >= 0; s--)
		{
			CSocketThread* pSocket = SocketArray[s];
			if (!pSocket)
				continue;

			pSocket->ExitInstance();
			delete pSocket;
			SocketArray.RemoveAt(s);
		}

	_critSect.Unlock();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSocketThread* CClientSocketBase::GetAt(int nItem)
{
	if (nItem < 0 || nItem >= GetCount())
		return 0;
	
	CSocketThread* pSocket = NULL;

	_critSect.Lock();
		pSocket = SocketArray[nItem];
	_critSect.Unlock();
	
	return pSocket;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CClientSocketBase::UpdateCloseEventArray()
{
	_critSect.Lock();
		
		int nSocketCount = (int) SocketArray.GetCount();
		for(int s = 0; s < nSocketCount ; s++)
		{
			CSocketThread* pSocket = SocketArray[s];
			if (!pSocket)
				continue;

			CloseEventArray[s] = pSocket->GetCloseEventHandle();
		}

	_critSect.Unlock();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------