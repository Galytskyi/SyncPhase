#pragma once
#include "CommThread.h"

// CSocketThread command target

// ======================================================================================

BOOL InitializeSocket();

// ======================================================================================

struct SOCKET_MSG
{
	int					MsgLen;	
	int					SourceID;

	SYSTEMTIME	TimeStamp;	

	int					CommMsgLen;	
	COMM_MSG		CommMsg;
	
	BYTE				CRC8;
};

// ======================================================================================

const int SOCKET_REQUEST_UNKNOWN											= -1,
					SOCKET_REQUEST_GET_PORT_IO									= 0,
					SOCKET_REQUEST_SET_DISCRETE_STATE						= 1,
					SOCKET_REQUEST_GET_EEPROM_PARAM							= 2,
					SOCKET_REQUEST_SET_EEPROM_PARAM							= 3;

const int SOCKET_REQUEST_COUNT												= 4;

struct SOCKET_REQUEST
{
	int						MsgLen;	
	int						SourceID;
	COMM_REQUEST	CommRequest;
};


// ======================================================================================

class CSocketMsgQueue : public CObject
{
private:
	CCriticalSection	_critSect;

	CList<SOCKET_MSG, SOCKET_MSG&>	SocketMsgList;
			
public:
	CSocketMsgQueue();
	virtual ~CSocketMsgQueue();

	int						GetCount();

	void					Push(SOCKET_MSG Msg);
	void					Push(int SourceID, COMM_MSG CommMsg, int CommMsgLen);
	SOCKET_MSG	 	Pop();

	BOOL					ExitRemoveMsgThread;
	CWinThread*		pRemoveMsgThread;
	static UINT		RemoveMsgThread(LPVOID pParam);

	BOOL					ProcessingSocketMsg(SOCKET_MSG SocketMsg);
};

// ======================================================================================

const int SOCKET_DESCRIPTION_LEN	= 32; // 

// ======================================================================================

class CSocketThread : public CWinThread
{
	DECLARE_DYNAMIC(CSocketThread)

public:
	CSocketThread(int Type, DWORD ip, SOCKET socket = INVALID_SOCKET, CSocketThread* parent = NULL);
	virtual ~CSocketThread();

	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()

private:
	int							SocketType;
	DWORD						IP;
	SOCKET					hSocket;
	CSocketThread*	pParent;
		
	HANDLE					hSocketEvent;
	HANDLE					hCloseSocket;
		
	BOOL						bConnect;

	BOOL						CreateSocket();
	BOOL						CreateServerSocket();
	BOOL						CreateClientSocket();
	void						CloseSocket();

	BOOL						Accept();
	BOOL						Connect();

	BOOL						ExitEventThread;
	CWinThread*			pEventThread;
	static UINT			EventThread(LPVOID pParam);
	
	BOOL						ExitConnectThread;
	CWinThread*			pConnectThread;
	static UINT			ConnectThread(LPVOID pParam);

	BOOL						ExitWaitCloseSokcetThread;
	CWinThread*			pWaitCloseSocketThread;
	static UINT			WaitCloseSocketThread(LPVOID pParam);


	DWORD						RecvBytes;
	DWORD						SentBytes;
	
	SYSTEMTIME			TimeLiving;
	void						UpdateTimeLiving();

public:

	DWORD						GetIP() { return IP; };
	CString					GetSentBytesStr();
	TCHAR						Description[SOCKET_DESCRIPTION_LEN];
	CString					GetTimeLivingStr();
	
	HANDLE					GetCloseEventHandle() { return hCloseSocket; } ;

	SOCKET					GetSocketHandle() { return hSocket; } ;
	CSocketThread*	GetParentSocket() { return pParent; } ;
	
	BOOL						IsConnect() { return bConnect; };
	
	BOOL						Recv();
	BOOL						Send(BYTE* pBuffer, int Len);

	void						RequestGetPortIO(int SourceID);
	//void						RequestSetDiscreteState(BYTE Port, BYTE Input, BYTE State);
	//void						RequestGetMemoryParam();
	//void						RequestSetMemoryParam(BYTE Address, BYTE aByte);
	
};

// ======================================================================================

class CClientSocketBase : public CObject
{
private:
	CCriticalSection				_critSect;

	CArray<CSocketThread*>	SocketArray;

public:
	CClientSocketBase();
	virtual ~CClientSocketBase();

	HANDLE					CloseEventArray[0x100];

	int							GetCount();

	int							Add(CSocketThread* pSocket);
	
	BOOL						Remove(int nItem);
	BOOL						Remove(CSocketThread* pSocket);
	void						RemoveAll();

	CSocketThread*	GetAt(int nItem);

	void						UpdateCloseEventArray();
};


// ======================================================================================


