#include <avr/eeprom.h>

#include "Simulator.h"
#include "USART.h"
#include "EEPROM.h"


// ------------------------------------------------------------------------------

void InitEepromParam()
{
	BYTE Index = 0;
	while(Index < EEPROM_PARAM_COUNT)
		EEPROM_PARAM[Index] = eeprom_read_byte(Index++);
}

// ------------------------------------------------------------------------------

void SendEepromParam()
{
	InitEepromParam();

	SendData(EEPROM_PARAM, EEPROM_PARAM_COUNT, MSG_TYPE_DATA_EEPROM);
}

// ------------------------------------------------------------------------------

void SetEepromParam(BYTE Address, BYTE Value)
{
	eeprom_write_byte(Address, Value);
			
	InitEepromParam();

	SendData(EEPROM_PARAM, EEPROM_PARAM_COUNT, MSG_TYPE_DATA_EEPROM);
}

// ------------------------------------------------------------------------------
