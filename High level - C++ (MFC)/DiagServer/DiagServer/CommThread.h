#pragma once
#include "SourceItem.h"

// ======================================================================================

const int DISCRETE_PORT_COUNT				= 7;

const int DISCRETE_PORT_TYPE_IO			=	0,
					DISCRETE_PORT_TYPE_DATA		= 1,
					DISCRETE_PORT_TYPE_PIN		= 2;

const int DISCRETE_PORT_TYPE_COUNT	= 3;

const int DIAG_DISCRETE_ARRAY_LEN		= DISCRETE_PORT_COUNT * sizeof(BYTE);

const int ANALOG_INPUT_COUNT				= 4;

const int DIAG_ANALOG_ARRAY_LEN			= ANALOG_INPUT_COUNT * sizeof(WORD);

const int EEPROM_BYTE_COUNT					= 64;

// ======================================================================================

const int TIMEOUT_SECOND							= 2;	// 1 сек
const int TIMEOUT_COUNT								= 10; // число таймаутов для переинициализации 5сек

// ======================================================================================

const int COMM_PORT_STATE_NOT_OPENED	= 0,	
					COMM_PORT_STATE_OK					= 1;
										
const int COMM_PORT_STATE_COUNT				= 2;

const TCHAR* const CommStateStr[COMM_PORT_STATE_COUNT]	=
{
	_T("Порт не открыт"),
	_T("Есть"),
};

// ======================================================================================

struct COMM_STATE
{
	int						nCommState;
	CString				GetCommStateStr();

	DWORD					ReceivedBytes;
	BOOL					ReceivedBytesOk;
	CString				GetReceivedBytesStr();
	
	SYSTEMTIME		TimeLiving;
	CString				GetTimeLivingStr();

	int						ErrorCRC;

	COMM_STATE()
	{
		nCommState				= COMM_PORT_STATE_NOT_OPENED;
		ReceivedBytes			= 0;
		ReceivedBytesOk		= FALSE;
		ZeroMemory(&TimeLiving, sizeof(SYSTEMTIME));
		TimeLiving.wDay = 1; TimeLiving.wMonth = 1; TimeLiving.wYear = 1601; 
		ErrorCRC					= 0;
	}
};


// ======================================================================================

const int COMM_MSG_LEN															= 200;

// ======================================================================================

const int COMM_MSG_TYPE_STR													=	0,
					COMM_MSG_TYPE_DATA												=	1;

const int COMM_MSG_TYPE_STR_USIAL										=	0,
					COMM_MSG_TYPE_STR_WARNING									=	1,
					COMM_MSG_TYPE_STR_ERROR										=	2;

const int COMM_MSG_TYPE_DATA_PORT_IO								=	0,
					COMM_MSG_TYPE_DATA_PORT_STATE							=	1,
					COMM_MSG_TYPE_DATA_EEPROM									=	2;
					

// ======================================================================================

union COMM_MSG_HEADER
{
	struct 
	{
		BYTE	StrType;
		char	Str[COMM_MSG_LEN];
	} StrHeader;

	struct 
	{
		BYTE	Len;
		BYTE	CRC8;
		BYTE	DataType;
		BYTE	Data[COMM_MSG_LEN];
	} DataHeader;
};

// ======================================================================================

struct COMM_MSG
{
	BYTE							Type;
	COMM_MSG_HEADER		Header;
};

// ======================================================================================

const int COMM_REQUEST_UNKNOWN											= -1,
					COMM_REQUEST_GET_PORT_IO									= 0,
					COMM_REQUEST_SET_DISCRETE_STATE						= 1,
					COMM_REQUEST_GET_EEPROM_PARAM							= 2,
					COMM_REQUEST_SET_EEPROM_PARAM							= 3;

const int COMM_REQUEST_COUNT												= 4;

const int COMM_REQUEST_DATA_LEN											= 4;

struct COMM_REQUEST
{
	BYTE	Type;
	BYTE	DataLen;
	BYTE	CRC8;
	BYTE	Data[COMM_REQUEST_DATA_LEN];
	BYTE	End;
	
	COMM_REQUEST()
	{
		Type = COMM_REQUEST_UNKNOWN;
		DataLen = COMM_REQUEST_DATA_LEN;
		ZeroMemory(&Data, COMM_REQUEST_DATA_LEN);
		End = 0x0D;
	}
};

// ======================================================================================

// CCommThread

class CSourceItem;

class CCommThread : public CWinThread
{
	DECLARE_DYNCREATE(CCommThread)

public:

	CCommThread() {} ;           
	CCommThread(CSourceItem* Source);           // protected constructor used by dynamic creation
	virtual ~CCommThread();

	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CSourceItem*	pSource;
	
	HANDLE				hComm;

	void					OpenPort();
	void					ClosePort();

	CWinThread*		pTimeoutThread;
	BOOL					ExitTimeoutThread;
	static UINT		TimeoutThread(LPVOID pParam);

	void					UpdateTimeLiving();

	BOOL					InitComm();
	void					OnReadEvent() ;

	CWinThread*		pReadMsgThread;
	BOOL					ExitMsgThread;
	static UINT		ReadMsgThread(LPVOID pParam);


	BOOL					Send(BYTE* pBuffer, int Len);

public:

	void					ReConnect();

	COMM_STATE		CommState;

	void					RequestGetPortIO();
	void					RequestSetDiscreteState(BYTE Port, BYTE Input, BYTE State);
	void					RequestGetMemoryParam();
	void					RequestSetMemoryParam(BYTE Address, BYTE aByte);
};

// ======================================================================================
