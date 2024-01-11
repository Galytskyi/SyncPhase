#include <avr/io.h>
#include <avr/interrupt.h>

#include "Simulator.h"
#include "USART.h"
#include "DiagPorts.h"
#include "EEPROM.h"
#include "crc8.h"

// ------------------------------------------------------------------------------

void InitUSART()
{	
	// устанвливаем скороть обмена

	UBRR0H = (BYTE)(USART_UBBR_VALUE>>8);
	UBRR0L = (BYTE)USART_UBBR_VALUE;

	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);	// разрешаем чтение и передачу 
	UCSR0C = (0<<USBS0)|(1<<UCSZ1)|(1<<UCSZ0); 	// 8 bits, no parity, 1 stop bit
}


// ------------------------------------------------------------------------------

void SendByte(BYTE aByte)
{

	while((UCSR0A & (1<<UDRE0)) == 0); 	// ждем пока осводоится\ очередь передачи
	UDR0 = aByte;  											// переадем байт
}

// ------------------------------------------------------------------------------

BYTE ReceiveByte()
{
	while((UCSR0A & (1<<RXC0)) == 0);		// Ждем пока байт будет принят
	return UDR0;												// возвращаем принятый байт

}

// ------------------------------------------------------------------------------

BYTE 	RecvBuffer[USART_RECV_BUFFER_LEN];
BYTE	RecvIndex = 0;


// ------------------------------------------------------------------------------
// Прерывание на приходящий запрос
// RecvBuffer[0] - Type
// RecvBuffer[1] - DataLen
// RecvBuffer[2] - CRC8
// RecvBuffer[3] - Data[REQUEST_DATA_LEN];
// RecvBuffer[7] - End - 0x0D

ISR(SIG_UART0_RECV)
{
	if (RecvIndex >= USART_RECV_BUFFER_LEN)
	{
		RecvIndex = 0;
		return;
	}

	RecvBuffer[RecvIndex] = UDR0; 
	if (RecvBuffer[RecvIndex] == 0x0D)
	{
		// Проверка на размер запроса
		if (RecvIndex != REQUEST_LEN - 1)
		{
			RecvIndex++;
			return;
		}

		// Проверяем CRC8 пришедшего пакета 
		if (RecvBuffer[2] != CalcCRC8(&RecvBuffer[3], REQUEST_DATA_LEN))
		{
			RecvIndex = 0;
			return;
		}

		// Запрещаем прерывание по переполнению таймера
		// Выключая тем самым диагностику
		TIMSK	&= ~(1<<TOIE0);	


		// REQUEST_GET_PORT_IO -------------------------------------------------------
		if (RecvBuffer[0] == REQUEST_GET_PORT_IO)	
			SendPortIO();


		// REQUEST_SET_DISCRETE_STATE ------------------------------------------------
		// RecvBuffer[1] - Номер порта
		// RecvBuffer[2] - Номер входа
		// RecvBuffer[3] - Состояние - Нет(0)/Да(1)
		if (RecvBuffer[0] == REQUEST_SET_DISCRETE_STATE)	
			SetDiscreteInputState(RecvBuffer[3], RecvBuffer[4], RecvBuffer[5]);



		// REQUEST_GET_EEPROM_PARAM --------------------------------------------------
		if (RecvBuffer[0] == REQUEST_GET_EEPROM_PARAM)	
			SendEepromParam();

		// REQUEST_SET_EEPROM_PARAM --------------------------------------------------
		// RecvBuffer[1] - Адрес памяти
		// RecvBuffer[2] - Занчение
		if (RecvBuffer[0] == REQUEST_SET_EEPROM_PARAM)	
			SetEepromParam(RecvBuffer[3], RecvBuffer[4]);


		// Разрешаем прерывание по переполнению таймера
		// Включая тем самым диагностику
		TIMSK	|= (1<<TOIE0);


	
		RecvIndex = 0;
		
	}
	else
		RecvIndex++; 

}

// ------------------------------------------------------------------------------

void SendStr(char* pStr, BYTE StrType)
{	
	while((UCSR0A & (1<<UDRE0)) == 0); 		// ждем пока осводоится очередь передачи
	UDR0 = MSG_TYPE_STR;  								// переадем тип сообщения строка

	while((UCSR0A & (1<<UDRE0)) == 0); 		// ждем пока осводоится очередь передачи
	UDR0 = StrType;  											// переадем тип стоки

	while(*pStr)
	{
		while((UCSR0A & (1<<UDRE0)) == 0);	// ждем пока осводоится очередь передачи
		UDR0 = *pStr++; 										// переадем очередной байт сообщения
	}	

	while((UCSR0A & (1<<UDRE0)) == 0); 		// ждем пока осводоится очередь передачи
	UDR0 = 0x0D;  												// переадем байт-признак конец сообщения



}

// ------------------------------------------------------------------------------

void SendData(BYTE* pBuffer, BYTE BufferLength, BYTE DataType)
{
	while((UCSR0A & (1<<UDRE0)) == 0); 			// ждем пока осводоится очередь передачи
	UDR0 = MSG_TYPE_DATA;  									// переадем тип сообщения данные

	while((UCSR0A & (1<<UDRE0)) == 0); 			// ждем пока осводоится очередь передачи
	UDR0 = BufferLength;  									// переадем тип сообщения данные
	
	while((UCSR0A & (1<<UDRE0)) == 0); 			// ждем пока осводоится очередь передачи
	UDR0 = CalcCRC8(pBuffer, BufferLength); // переадем CRC8

	while((UCSR0A & (1<<UDRE0)) == 0); 			// ждем пока осводоится очередь передачи
	UDR0 = DataType;  											// переадем тип данныx
	
	while(BufferLength)
	{
		while((UCSR0A & (1<<UDRE0)) == 0);		// ждем пока осводоится очередь передачи
		UDR0 = *pBuffer++; 										// переадем очередной байт сообщения

		BufferLength --;
	}

	while((UCSR0A & (1<<UDRE0)) == 0); 			// ждем пока осводоится очередь передачи
	UDR0 = 0x0D;  													// переадем байт-признак конец сообщения

}


// ------------------------------------------------------------------------------
