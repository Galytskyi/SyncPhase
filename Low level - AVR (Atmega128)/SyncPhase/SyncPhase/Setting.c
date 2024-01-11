#include "Setting.h"
#include<avr/eeprom.h>

// -------------------------------------------------------------------------------

char* Unit[UNIT_COUNT] = 
{
	"",
	"Гц",
	"мкс"
};

// -------------------------------------------------------------------------------

char* SettingDescription[SETTING_COUNT] = 
{
	"Частота питания",
	"Таймаут тиристора"
}; 

SETTING Settings[SETTING_COUNT] = 
{
	{0.0, 0, 45, 55, UNIT_HZ, 1}, 		// Частота питания, Гц
	{0.0, 0, 10, 40, UNIT_MKS, 0}, 		// Таймаут тиристора, мкс
};

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

uint ReadSetting(byte nSetting)
{
	if (nSetting >= SETTING_COUNT)
		return 0;

	uint addr = nSetting * 2;

	uint Value = eeprom_read_word((const uint*)addr);

	Settings[nSetting].PhysicalVal 	= GetSettingPhysicalVal(nSetting, Value);
	Settings[nSetting].ADCVal 			= Value;

	return Value;
}

// -------------------------------------------------------------------------------

byte WriteSetting(byte nSetting, uint Value)
{
	if (nSetting >= SETTING_COUNT)
		return 0;

	uint addr = nSetting * 2;

	eeprom_write_word((const uint*)addr, Value);

	Settings[nSetting].PhysicalVal 	= GetSettingPhysicalVal(nSetting, Value);
	Settings[nSetting].ADCVal 			= Value;

	return 1;
}

// -------------------------------------------------------------------------------

void LoadSettings()
{
	int s = 0;

	for (s = 0; s < SETTING_COUNT; s ++)
		ReadSetting(s);
}

// -------------------------------------------------------------------------------

uint GetSettingADCVal(byte nSetting, double PhysicalVal)
{
	if (nSetting >= SETTING_COUNT)
		return 0;

	SETTING Setting = Settings[nSetting];

	return 	0xFFFF*(PhysicalVal - Setting.Low) / (Setting.High - Setting.Low);
}

// -------------------------------------------------------------------------------

double GetSettingPhysicalVal(byte nSetting, uint ADCVal)
{
	if (nSetting >= SETTING_COUNT)
		return 0.0;

	SETTING Setting = Settings[nSetting];

	return (Setting.High - Setting.Low) * ADCVal / (double)(0xFFFF) + Setting.Low;
					
}

// -------------------------------------------------------------------------------
