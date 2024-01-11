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

	// Инициализаци COM-порта
	InitUSART();

	// Назначаем вход/выход портов
	
	DDRC 	= 0xFF;
	DDRG 	= 0xFF;

	// Оптравляем состояние портов
	SendPortIO();
	SendEepromParam();

	//Запускаем диагностику портов
	StartDiagPorts();

	// Бесконечный цикл для программы
	while(TRUE)
	{
		BYTE nDelay = 0;
		
		SendStr("Ожидаем нажатие кнопки \"Пуск\"", MSG_TYPE_STR_WARNING);

		while((PINA & 1) == 0);

		SendStr("Кнопка \"Пуск\" нажата", MSG_TYPE_STR_USUAL);

		SendStr("Задержка в секундах \"Параметр 2\" начата", MSG_TYPE_STR_USUAL);
		
		nDelay = EEPROM_PARAM[1];
		while(nDelay --)
			delay(1000);
		
		SendStr("Задержка в секундах \"Параметр 2\" окончена", MSG_TYPE_STR_USUAL);

		SendStr("Если \"Параметр 3\" равен 0 Горит 2-й вход порта С", MSG_TYPE_STR_USUAL);
		SendStr("Если \"Параметр 3\" не равен 0 Горит 3-й вход порта С", MSG_TYPE_STR_USUAL);

		if (EEPROM_PARAM[2])
			PORTC ^= 0x2; 
		else
			PORTC ^= 0x4; 


		SendStr("Задержка в секундах \"Параметр 2\" окончена", MSG_TYPE_STR_USUAL);
		SendStr("Тушим порт С", MSG_TYPE_STR_USUAL);

		PORTC = 0;

		SendStr("Цикл окончен", MSG_TYPE_STR_ERROR);

	};




}

// ------------------------------------------------------------------------------


