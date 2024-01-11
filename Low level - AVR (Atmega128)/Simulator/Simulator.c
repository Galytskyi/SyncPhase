#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Simulator.h"
#include "USART.h"
#include "DiagPorts.h"
#include "EEPROM.h"

// ------------------------------------------------------------------------------

void delay(WORD ms)
{
	while(ms--)
	{	
		WORD tick = 2000; 
		while(tick --)
			asm volatile ( "nop\n" ::);
	}
		
}

int main(void)
{
	sei();

	// ������������ COM-�����
	InitUSART();

	// ��������� ����/����� ������
	
	DDRC 	= 0xFF;
	DDRG 	= 0xFF;

	// ���������� ��������� ������
	SendPortIO();
	SendEepromParam();

	//��������� ����������� ������
	StartDiagPorts();

	// ����������� ���� ��� ���������
	while(TRUE)
	{
		BYTE nDelay = 0;
		
		SendStr("������� ������� ������ \"����\"", MSG_TYPE_STR_WARNING);

		while((PINA & 1) == 0);

		SendStr("������ \"����\" ������", MSG_TYPE_STR_USUAL);

		SendStr("�������� � �������� \"�������� 2\" ������", MSG_TYPE_STR_USUAL);
		
		nDelay = EEPROM_PARAM[1];
		while(nDelay --)
			delay(1000);
		
		SendStr("�������� � �������� \"�������� 2\" ��������", MSG_TYPE_STR_USUAL);

		SendStr("���� \"�������� 3\" ����� 0 ����� 2-� ���� ����� �", MSG_TYPE_STR_USUAL);
		SendStr("���� \"�������� 3\" �� ����� 0 ����� 3-� ���� ����� �", MSG_TYPE_STR_USUAL);

		if (EEPROM_PARAM[2])
			PORTC ^= 0x2; 
		else
			PORTC ^= 0x4; 


		SendStr("�������� � �������� \"�������� 2\" ��������", MSG_TYPE_STR_USUAL);
		SendStr("����� ���� �", MSG_TYPE_STR_USUAL);

		PORTC = 0;

		SendStr("���� �������", MSG_TYPE_STR_ERROR);

	};




}

// ------------------------------------------------------------------------------


