// SourceItem.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "SourceItem.h"
#include "DataFile.h"

#include "..\..\INCLUDE\util.h"

// CMsgStrArray
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

CMsgStrArray::CMsgStrArray()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CMsgStrArray::~CMsgStrArray()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CMsgStrArray::GetCount()
{
	int nItemCount = 0;

	_critSect.Lock();
		nItemCount = (int) MsgArray.GetCount();
	_critSect.Unlock();

	return nItemCount;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CMsgStrArray::Add(DIAG_MSG_STR Msg)
{
	int nItem = -1;

	_critSect.Lock();
		nItem = MsgArray.Add(Msg);
	_critSect.Unlock();

	return nItem;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CMsgStrArray::RemoveAll()
{
	_critSect.Lock();
		MsgArray.RemoveAll();	
	_critSect.Unlock();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

DIAG_MSG_STR CMsgStrArray::GetAt(int nItem)
{
	DIAG_MSG_STR Msg;

	if (nItem < 0 || nItem >= GetCount())
		return Msg;
	
	_critSect.Lock();
		Msg = MsgArray[nItem];
	_critSect.Unlock();
	
	return Msg;
}

// CSourceItem
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceItem::CSourceItem()
: ID (-1)
, pCommThread( NULL )
, EnableRecvMsg(FALSE)
, AutoEnsureMsg(FALSE)
{
	Comm[0] = 0;
	Description[0] = 0;
	ZeroMemory(&PortIO, DISCRETE_PORT_COUNT * sizeof(BYTE));
	ZeroMemory(&DiscretePortState, DISCRETE_PORT_COUNT * sizeof(BYTE));
	ZeroMemory(&AnalogInputState, ANALOG_INPUT_COUNT * sizeof(WORD));
	ZeroMemory(&MemoryByteState, EEPROM_BYTE_COUNT * sizeof(BYTE));
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceItem::CSourceItem(int nID, CString strComm, CString strDescription)
: ID (nID)
, pCommThread( NULL )
, EnableRecvMsg(FALSE)
, AutoEnsureMsg(FALSE)
{
	SafeStrncpy(Comm, strComm, SOURCE_COMM_LEN );	
	SafeStrncpy(Description, strDescription, SOURCE_DESCRIPTION_LEN );	

	ZeroMemory(&PortIO, DISCRETE_PORT_COUNT * sizeof(BYTE));
	ZeroMemory(&DiscretePortState, DISCRETE_PORT_COUNT * sizeof(BYTE));
	ZeroMemory(&AnalogInputState, ANALOG_INPUT_COUNT * sizeof(WORD));
	ZeroMemory(&MemoryByteState, EEPROM_BYTE_COUNT * sizeof(BYTE));

	pCommThread = new CCommThread(this);
	if (pCommThread)
		pCommThread->CreateThread();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceItem::~CSourceItem()
{
	if (pCommThread)
	{
		pCommThread->ExitInstance();
		delete pCommThread;
		pCommThread = NULL;
	}

	MsgStrArray.RemoveAll();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CString CSourceItem::GetState()
{
	if (!pCommThread)
		return _T("Не создан поток COM-порта");
	else
		return pCommThread->CommState.GetCommStateStr();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

// CSourceBase
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceBase::CSourceBase()
	: pActiveSource(NULL)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceBase::~CSourceBase()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CSourceBase::GetCount()
{
	int nItemCount = 0;

	_critSect.Lock();
		nItemCount = (int) SourceArray.GetCount();
	_critSect.Unlock();

	return nItemCount;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CSourceBase::Add(CSourceItem* pSource)
{
	if(!pSource)
		return -1;

	int nItem = -1;

	_critSect.Lock();
		nItem = SourceArray.Add(pSource);
	_critSect.Unlock();
	
	return nItem;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSourceBase::Remove(int nItem)
{
	if (nItem < 0 || nItem >= GetCount())
		return FALSE;
	
	_critSect.Lock();

		CSourceItem* pSource = SourceArray[nItem];
		if (pSource)
		{
			delete pSource;
			SourceArray.RemoveAt(nItem);
		}

	_critSect.Unlock();
	
	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSourceBase::Remove(CSourceItem* pSource)
{
	if (!pSource)
		return FALSE;

	_critSect.Lock();

		int nSourceCount = (int) SourceArray.GetCount();
		for(int s = 0; s < nSourceCount ; s++)
		{
			CSourceItem* pNSource = SourceArray[s];
			if (!pNSource)
				continue;

			if (pNSource == pSource)
			{
				delete pSource;
				SourceArray.RemoveAt(s);
				break;
			}
		}

	_critSect.Unlock();

	

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSourceBase::RemoveAll()
{
	_critSect.Lock();

		int nSourceCount = (int) SourceArray.GetCount();
		for(int s = nSourceCount - 1; s >= 0; s--)
		{
			CSourceItem* pSource = SourceArray[s];
			if (!pSource)
				continue;

			delete pSource;
			SourceArray.RemoveAt(s);
		}

	_critSect.Unlock();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceItem* CSourceBase::GetAt(int nItem)
{
	if (nItem < 0 || nItem >= GetCount())
		return 0;
	
	CSourceItem* pSource = NULL;

	_critSect.Lock();
		pSource = SourceArray[nItem];
	_critSect.Unlock();
	
	return pSource;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSourceBase::CheckComm(CString strComm)
{
	BOOL bNotFound = TRUE;

	_critSect.Lock();

		int nSourceCount = (int) SourceArray.GetCount();
		for(int s = 0; s < nSourceCount ; s++)
		{
			CSourceItem* pSource = SourceArray[s];
			if (!pSource)
				continue;

			if (!strComm.CompareNoCase(pSource->Comm))
			{
				bNotFound = FALSE;
				break;
			}
		}
	_critSect.Unlock();

	return bNotFound;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CSourceItem* CSourceBase::GetActiveSource()
{
	CSourceItem* pSource = NULL;

	_critSect.Lock();
		pSource = pActiveSource;
	_critSect.Unlock();

	return pSource;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSourceBase::SetActiveSource(CSourceItem* pSource)
{
	_critSect.Lock();
		pActiveSource = pSource;
	_critSect.Unlock();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSourceBase::Load()
{
	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_SOURCE))
		return;

	int SourceID = 0;
	CSourceItem Source;

	
	_critSect.Lock();
	
		int nRecordCount = Table.GetRecordCount();
		for (int r = 0; r < nRecordCount; r++)
		{
			if (!Table.ReadRecord(r, &Source))
				continue;

			CSourceItem* pSource = new CSourceItem(SourceID++, Source.Comm, Source.Description);
			if (!pSource)
				continue;

			SourceArray.Add(pSource);
		}

	_critSect.Unlock();

	Table.Close();

}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CSourceBase::Save()
{
	CDataFile Table;
	if (!Table.Open(TABLE_TYPE_SOURCE))
		return;

	Table.Empty();

	_critSect.Lock();
	
		int nSourceCount = (int) SourceArray.GetCount();
		for(int s = 0; s < nSourceCount ; s++)
		{
			CSourceItem* pSource = SourceArray[s];
			if (!pSource)
				continue;

			Table.WriteRecord(pSource);
		}

	_critSect.Unlock();

	Table.Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------