

// ----------------------------------------------------------

#define EEPROM_PARAM_COUNT 64

BYTE 		EEPROM_PARAM[EEPROM_PARAM_COUNT];

// ----------------------------------------------------------

void 		InitEepromParam();

void 		SendEepromParam();

void 		SetEepromParam(BYTE Address, BYTE Value);

// ----------------------------------------------------------
