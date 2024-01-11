#ifndef _SETTING_H_
#define _SETTING_H_

#include "SyncPhase.h"


#define				UNIT_NONE							0
#define				UNIT_HZ 							1
#define				UNIT_MKS 							2

#define				UNIT_COUNT 						3

extern				char* Unit[UNIT_COUNT];



#define 			SETTING_F_POWER 			0
#define 			SETTING_T_TIRISTOR 		1

#define 			SETTING_COUNT 				2

extern char* 	SettingDescription[SETTING_COUNT]; 

typedef struct _SETTING
{
	double 	PhysicalVal;		// ���������� ������� �������
	uint		ADCVal;					// ������������� �������� �������

	double 	Low;						// ������ �������� ����������� ���������
	double 	High;						// ������� ������� ����������� ���������
 	
	byte 		Unit;						// ������� ���������

	byte		Precise;				// ��������, �-�� ������ ����� �������

} SETTING;


extern SETTING 	Settings[SETTING_COUNT]; 


uint						ReadSetting(byte nSetting);
byte						WriteSetting(byte nSetting, uint Value);

void 						LoadSettings();

uint 						GetSettingADCVal(byte nSetting, double PhysicalVal);
double 					GetSettingPhysicalVal(byte nSetting, uint ADCVal);

#endif /* _SETTING_H_ */
