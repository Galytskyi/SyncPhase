#include "stdafx.h"
#include "DiagServer.h"
#include "DataFile.h"
#include "SourceItem.h"

#include "DlgTabDiagDiscrete.h"
#include "DlgTabDiagAnalog.h"
#include "DlgTabMemory.h"


// -------------------------------------------------------------------------------------------------------------------------------------------------

CDataFile::CDataFile()
:	m_TableType(TABLE_TYPE_UNKNONW)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDataFile::~CDataFile(void)
{
	Close();
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDataFile::Open(int TableType, TCHAR* strTableName)
{
	if (ERR_TABLE_TYPE(TableType))
		return FALSE;

	CString strFieldName;
	
	switch(TableType)
	{
		case TABLE_TYPE_SOURCE:
		
			m_Table.AddField(_T("SOURCE_ID"),			TYPE_INTEGER);
			m_Table.AddField(_T("COMM"),					TYPE_CHAR, SOURCE_COMM_LEN);
			m_Table.AddField(_T("NAME"),					TYPE_CHAR, SOURCE_DESCRIPTION_LEN);

			break;

		case TABLE_TYPE_DIAG_MSG_STR:

			m_Table.AddField(_T("SOURCE_ID"),			TYPE_INTEGER);
			m_Table.AddField(_T("TIME"),					TYPE_CHAR, DIAG_MSG_TIME_LEN);
			m_Table.AddField(_T("DATE"),					TYPE_CHAR, DIAG_MSG_DATE_LEN);
			m_Table.AddField(_T("TIME_CODE"),			TYPE_INTEGER);
			m_Table.AddField(_T("MSG_TYPE"),			TYPE_CHAR, DIAG_MSG_STR_LEN);
			m_Table.AddField(_T("MSG"),						TYPE_CHAR, DIAG_MSG_STR_LEN);

			break;

		case TABLE_TYPE_DIAG_MSG_INPUT:

			m_Table.AddField(_T("SOURCE_ID"),			TYPE_INTEGER);
			m_Table.AddField(_T("TIME"),					TYPE_CHAR, DIAG_MSG_TIME_LEN);
			m_Table.AddField(_T("DATE"),					TYPE_CHAR, DIAG_MSG_DATE_LEN);
			m_Table.AddField(_T("TIME_CODE"),			TYPE_INTEGER);
			m_Table.AddField(_T("CONTACT"),				TYPE_INTEGER);
			m_Table.AddField(_T("STATE"),					TYPE_INTEGER);

			break;
		
		case TABLE_TYPE_DISCRETE_INPUT:

			m_Table.AddField(_T("D_ID"),					TYPE_INTEGER);
			m_Table.AddField(_T("CONTACT"),				TYPE_INTEGER);
			m_Table.AddField(_T("NAME"),					TYPE_CHAR, ANALOG_INPUT_DESCRIPTION_LEN);
			m_Table.AddField(_T("IO"),						TYPE_INTEGER);
			m_Table.AddField(_T("PORT"),					TYPE_INTEGER);
			m_Table.AddField(_T("INPUT"),					TYPE_INTEGER);
			m_Table.AddField(_T("SIG_TYPE"),			TYPE_INTEGER);

			break;

		case TABLE_TYPE_ANALOG_INPUT:

			m_Table.AddField(_T("A_ID"),					TYPE_INTEGER);
			m_Table.AddField(_T("CONTACT"),				TYPE_INTEGER);
			m_Table.AddField(_T("NAME"),					TYPE_CHAR, ANALOG_INPUT_DESCRIPTION_LEN);
			m_Table.AddField(_T("PRECISE"),				TYPE_INTEGER);
			m_Table.AddField(_T("LOW_LIMIT"),			TYPE_DOUBLE);
			m_Table.AddField(_T("HIGH_LIMIT"),		TYPE_DOUBLE);
			m_Table.AddField(_T("UNIT"),					TYPE_CHAR, ANALOG_INPUT_UNIT_LEN);

			break;
			
		case TABLE_TYPE_EEPROM_PARAM:

			m_Table.AddField(_T("VALUE"),					TYPE_INTEGER);
			m_Table.AddField(_T("TYPE"),					TYPE_INTEGER);
			m_Table.AddField(_T("NAME"),					TYPE_CHAR, EEPROM_PARAM_DESCRIPTION_LEN);

			break;

	}

	m_TableType = TableType;

	TCHAR PathTableName[MAX_PATH];
	
	if (strTableName)
		_tcscpy_s( PathTableName, strTableName);
	else
	{
		_tcscpy_s(PathTableName, theApp.CurrentDirectoryStr);

		_tcscat_s(PathTableName, POSTFIX_DATAFILE);
		_tcscat_s(PathTableName, _T("\\"));
		_tcscat_s(PathTableName, TableNameStr[TableType]);
	}
	
	return m_Table.Open(PathTableName);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDataFile::ReadRecord(int nRecord, LPVOID pRecord)
{
	if (nRecord < 0 || nRecord >= GetRecordCount() )
		return FALSE;

	if (!pRecord)
		return FALSE;

	CSourceItem*					pSource							= NULL;
	DIAG_MSG_STR*					pDiagMsgStr					= NULL;
	DIAG_MSG_INPUT*				pDiagMsgInput				= NULL;
	DISCRETE_INPUT*				pDiscreteInput			= NULL;
	ANALOG_INPUT*					pAnalogInput				= NULL;
	EEPROM_PARAM*					pMemoryParam				= NULL;
	
	int										nField							= 0;

	switch(m_TableType)
	{
		case TABLE_TYPE_SOURCE:
		
			pSource = (CSourceItem*) pRecord;

			m_Table.ReadValue(nRecord,	 nField++,	pSource->ID);
			m_Table.ReadValue(nRecord,	 nField++,	pSource->Comm, SOURCE_COMM_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pSource->Description, SOURCE_DESCRIPTION_LEN);
						
			break;

		case TABLE_TYPE_DIAG_MSG_STR:
		
			pDiagMsgStr = (DIAG_MSG_STR*) pRecord;

			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgStr->SourceID);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgStr->TimeStr, DIAG_MSG_TIME_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgStr->DateStr, DIAG_MSG_DATE_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgStr->dwTimeCode.TimeCode);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgStr->StrType);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgStr->MsgStr, DIAG_MSG_STR_LEN);
			
			break;

		case TABLE_TYPE_DIAG_MSG_INPUT:
		
			pDiagMsgInput = (DIAG_MSG_INPUT*) pRecord;

			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgInput->SourceID);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgInput->TimeStr, DIAG_MSG_TIME_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgInput->DateStr, DIAG_MSG_DATE_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgInput->dwTimeCode.TimeCode);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgInput->Contact);
			m_Table.ReadValue(nRecord,	 nField++,	pDiagMsgInput->State);

			break;

		case TABLE_TYPE_DISCRETE_INPUT:
		
			pDiscreteInput = (DISCRETE_INPUT*) pRecord;

			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->No);
			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->Contact);
			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->Description, ANALOG_INPUT_DESCRIPTION_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->IO);
			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->Port);
			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->Input);
			m_Table.ReadValue(nRecord,	 nField++,	pDiscreteInput->SignalingType);
			
			break;

		case TABLE_TYPE_ANALOG_INPUT:
		
			pAnalogInput = (ANALOG_INPUT*) pRecord;

			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->No);
			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->Contact);
			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->Description, ANALOG_INPUT_DESCRIPTION_LEN);
			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->Precise);
			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->LowLimit);
			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->HighLimit);
			m_Table.ReadValue(nRecord,	 nField++,	pAnalogInput->Unit, ANALOG_INPUT_UNIT_LEN);
			
			break;

		case TABLE_TYPE_EEPROM_PARAM:
		
			pMemoryParam = (EEPROM_PARAM*) pRecord;

			m_Table.ReadValue(nRecord,	 nField++,	pMemoryParam->Value);
			m_Table.ReadValue(nRecord,	 nField++,	pMemoryParam->Type);
			m_Table.ReadValue(nRecord,	 nField++,	pMemoryParam->Description, EEPROM_PARAM_DESCRIPTION_LEN);
			
			break;
	}

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

int CDataFile::WriteRecord(LPVOID pRecord, int nRecord)
{
	if (!pRecord)
		return BAD_RECORD;

	// если номер записи указан значит эту запись обновим 
	// если номер записи -1 (BAD_RECORD) значит надо добавить новую запись
	if (nRecord == BAD_RECORD)
		nRecord	= m_Table.AddRecord();;

	if (nRecord < 0 || nRecord >= GetRecordCount() )
		return BAD_RECORD;

	CSourceItem*					pSource							= NULL;
	DIAG_MSG_STR*					pDiagMsgStr					= NULL;
	DIAG_MSG_INPUT*				pDiagMsgInput				= NULL;
	DISCRETE_INPUT*				pDiscreteInput			= NULL;
	ANALOG_INPUT*					pAnalogInput				= NULL;
	EEPROM_PARAM*					pMemoryParam				= NULL;
	
	int										nField							= 0;

	switch(m_TableType)
	{
		case TABLE_TYPE_SOURCE:
		
			pSource = (CSourceItem*) pRecord;

			m_Table.WriteValue(nRecord,	 nField++,	pSource->ID);
			m_Table.WriteValue(nRecord,	 nField++,	pSource->Comm);
			m_Table.WriteValue(nRecord,	 nField++,	pSource->Description);
						
			break;

		case TABLE_TYPE_DIAG_MSG_STR:

			pDiagMsgStr = (DIAG_MSG_STR*) pRecord;

			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgStr->SourceID);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgStr->TimeStr);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgStr->DateStr);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgStr->dwTimeCode.TimeCode);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgStr->StrType);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgStr->MsgStr);

			break;

		case TABLE_TYPE_DIAG_MSG_INPUT:
		
			pDiagMsgInput = (DIAG_MSG_INPUT*) pRecord;

			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgInput->SourceID);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgInput->TimeStr);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgInput->DateStr);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgInput->dwTimeCode.TimeCode);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgInput->Contact);
			m_Table.WriteValue(nRecord,	 nField++,	pDiagMsgInput->State);

			break;

		case TABLE_TYPE_DISCRETE_INPUT:
		
			pDiscreteInput = (DISCRETE_INPUT*) pRecord;

			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->No);
			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->Contact);
			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->Description);
			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->IO);
			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->Port);
			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->Input);
			m_Table.WriteValue(nRecord,	 nField++,	pDiscreteInput->SignalingType);
			
			break;


		case TABLE_TYPE_ANALOG_INPUT:
		
			pAnalogInput = (ANALOG_INPUT*) pRecord;

			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->No);
			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->Contact);
			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->Description);
			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->Precise);
			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->LowLimit);
			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->HighLimit);
			m_Table.WriteValue(nRecord,	 nField++,	pAnalogInput->Unit);
			
			break;

		case TABLE_TYPE_EEPROM_PARAM:
		
			pMemoryParam = (EEPROM_PARAM*) pRecord;

			m_Table.WriteValue(nRecord,	 nField++,	pMemoryParam->Value);
			m_Table.WriteValue(nRecord,	 nField++,	pMemoryParam->Type);
			m_Table.WriteValue(nRecord,	 nField++,	pMemoryParam->Description);
			
			break;
	}
	
	return nRecord;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
