#pragma once

#include "..\..\INCLUDE\DbfFileEx.h"

// ======================================================================================

#define POSTFIX_DATAFILE _T("\\DBF")

// ======================================================================================

const int										TABLE_TYPE_UNKNONW							= -1,
														TABLE_TYPE_SOURCE								= 0,
														TABLE_TYPE_DIAG_MSG_STR					= 1,
														TABLE_TYPE_DIAG_MSG_INPUT				= 2,
														TABLE_TYPE_DISCRETE_INPUT				= 3,
														TABLE_TYPE_ANALOG_INPUT					= 4,
														TABLE_TYPE_EEPROM_PARAM					= 5;

const int										TABLE_TYPE_COUNT								= 6;

// ======================================================================================

const TCHAR* const TableNameStr[TABLE_TYPE_COUNT] =
{
	_T("Sources.dbf"),
	_T("MessageD.dbf"),
	_T("MessageI.dbf"),
	_T("InputD.dbf"),
	_T("InputA.dbf"),
	_T("EEPROM.dbf"),
};

// ======================================================================================

#define	ERR_TABLE_TYPE(TableType) (TableType < 0 || TableType >= TABLE_TYPE_COUNT)

// ======================================================================================

const int BAD_RECORD = -1;

// ======================================================================================

class CDataFile
{	
private:
	CDbfFileEx		m_Table;																									// обьект класса CDbfFileEx
	int						m_TableType;																								// иденификатор таблицы TABLE_ID_XXX

public:
	CDataFile();
	~CDataFile(void);

	BOOL				Open(int TableType, TCHAR* strTableName = NULL);							// Открывает таблицу по идентификатору или имени
	void				Close() { m_Table.Close(); };																// закрывет таблицу

	BOOL				Empty() { return m_Table.Empty(); } ;												// очищает содержимое таблицы	

	int					GetRecordCount() { return m_Table.GetRecordCount(); };			// возвращает количесвто записей в таблице
	
	BOOL				ReadRecord(int nRecord, LPVOID pRecord);										// читает указаную запись из таблицы
	int					WriteRecord(LPVOID pRecord, int nRecord = BAD_RECORD);			// добавляет или обновляет запись в таблице (завитсит от параметра nRecord)
};

// ======================================================================================
