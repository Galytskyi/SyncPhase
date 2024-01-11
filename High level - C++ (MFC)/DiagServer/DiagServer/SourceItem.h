#pragma once
#include "CommThread.h"
#include "SocketThread.h"


// CSourceItem command target

// ======================================================================================

union DIAG_MSG_TIME_CODE
{
	struct 
	{
		DWORD Second	: 6;	
		DWORD Minute	: 6;
    DWORD Hour		: 5;
		DWORD Day			: 5;	
		DWORD Month		: 4;
    DWORD Year		: 5;
  };

  int TimeCode;

	DIAG_MSG_TIME_CODE() : TimeCode(0) {};
	DIAG_MSG_TIME_CODE(SYSTEMTIME SysTime) : TimeCode(0)	{ SetTime(SysTime); SetDate(SysTime);	};
	DIAG_MSG_TIME_CODE(SYSTEMTIME SysTime, SYSTEMTIME SysDate) : TimeCode(0)	{ SetTime(SysTime); SetDate(SysDate);	};
	DIAG_MSG_TIME_CODE(CTime SysTime) : TimeCode(0)	{ SetTime(SysTime); SetDate(SysTime);	};
	DIAG_MSG_TIME_CODE(CTime SysTime, CTime SysDate) : TimeCode(0)	{ SetTime(SysTime); SetDate(SysDate);	};

	void SetTime(SYSTEMTIME SysTime)
	{
		Hour		= SysTime.wHour;
		Minute	= SysTime.wMinute;
		Second	= SysTime.wSecond;
	};

	void SetDate(SYSTEMTIME SysTime)
	{
		Year		= 2010 - SysTime.wYear;
		Month		= SysTime.wMonth;
		Day			= SysTime.wDay;
	};


	void SetTime(CTime SysTime)
	{
		Hour				= SysTime.GetHour();
		Minute			= SysTime.GetMinute();
		Second			= SysTime.GetSecond();
	};

	void SetDate(CTime SysTime)
	{
		Year		= 2010 - SysTime.GetYear();
		Month		= SysTime.GetMonth();
		Day			= SysTime.GetDay();
	};

};


// ======================================================================================

const int DIAG_MSG_TIME_LEN		= 13;
const int DIAG_MSG_DATE_LEN		= 11;
const int DIAG_MSG_STR_LEN		= 64;	//COMM_MSG_STR_LEN;

struct DIAG_MSG_STR
{
	int										SourceID;
	TCHAR									TimeStr[DIAG_MSG_TIME_LEN];
	TCHAR									DateStr[DIAG_MSG_DATE_LEN];
	DIAG_MSG_TIME_CODE		dwTimeCode;
	int										StrType;
	TCHAR									MsgStr[DIAG_MSG_STR_LEN];

	DIAG_MSG_STR()
	{
		SourceID = -1;
		TimeStr[0] = 0;
		DateStr[0] = 0;
		dwTimeCode.TimeCode = 0;
		StrType = COMM_MSG_TYPE_STR_USIAL;
		MsgStr[0] = 0;
	};
};

// ======================================================================================

struct DIAG_MSG_INPUT
{
	int										SourceID;
	SYSTEMTIME						LocalTime;	
	TCHAR									TimeStr[DIAG_MSG_TIME_LEN];
	TCHAR									DateStr[DIAG_MSG_DATE_LEN];
	DIAG_MSG_TIME_CODE		dwTimeCode;
	int										Contact;
	int										State;

	DIAG_MSG_INPUT()
	{
		SourceID = -1;
		ZeroMemory(&LocalTime, sizeof(SYSTEMTIME));
		TimeStr[0] = 0;
		DateStr[0] = 0;
		dwTimeCode.TimeCode = 0;
		Contact = 0;
		State = 0;
	};
};


// ======================================================================================

class CMsgStrArray : public CObject
{
private:
	CCriticalSection				_critSect;

	CArray<DIAG_MSG_STR, DIAG_MSG_STR&>		MsgArray;

public:
	CMsgStrArray();
	virtual ~CMsgStrArray();

	int						GetCount();

	int						Add(DIAG_MSG_STR Msg);
	void					RemoveAll();

	DIAG_MSG_STR	GetAt(int nItem);
};

// =============================================================================================

const int SOURCE_COMM_LEN												=	8;
const int SOURCE_DESCRIPTION_LEN								= 32;

// ======================================================================================

class CSourceItem : public CObject
{
public:
	CSourceItem();
	CSourceItem(int nID, CString strComm, CString strDescription);
	virtual ~CSourceItem();
	

	int						ID;
	TCHAR					Comm[SOURCE_COMM_LEN];
	TCHAR					Description[SOURCE_DESCRIPTION_LEN];

	CString				GetState();

	CCommThread*	pCommThread;
		
	CMsgStrArray	MsgStrArray;
	BOOL					EnableRecvMsg;
	BOOL					AutoEnsureMsg;

	BYTE					PortIO[DISCRETE_PORT_COUNT];
	BYTE					DiscretePortState[DISCRETE_PORT_COUNT];
	WORD					AnalogInputState[ANALOG_INPUT_COUNT];
	BYTE					MemoryByteState[EEPROM_BYTE_COUNT];
};


// ========================================================================================

class CSourceBase : public CObject
{
private:
	CCriticalSection				_critSect;

	CArray<CSourceItem*>		SourceArray;

	CSourceItem*						pActiveSource;

public:
	CSourceBase();
	virtual ~CSourceBase();

	int						GetCount();

	int						Add(CSourceItem* pSource);
	
	BOOL					Remove(int nItem);
	BOOL					Remove(CSourceItem* pSource);
	void					RemoveAll();

	CSourceItem*	GetAt(int nItem);

	BOOL					CheckComm(CString strComm);

	CSourceItem*	GetActiveSource();
	void					SetActiveSource(CSourceItem* pSource);
	
	void					Load();
	void					Save();

};


// ======================================================================================

