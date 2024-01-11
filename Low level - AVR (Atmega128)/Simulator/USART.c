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
	// ������������ ������� ������

	UBRR0H = (BYTE)(USART_UBBR_VALUE>>8);
	UBRR0L = (BYTE)USART_UBBR_VALUE;

	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);	// ��������� ������ � �������� 
	UCSR0C = (0<<USBS0)|(1<<UCSZ1)|(1<<UCSZ0); 	// 8 bits, no parity, 1 stop bit
}


// ------------------------------------------------------------------------------

void SendByte(BYTE aByte)
{

	while((UCSR0A & (1<<UDRE0)) == 0); 	// ���� ���� ����������\ ������� ��������
	UDR0 = aByte;  											// �������� ����
}

// ------------------------------------------------------------------------------

BYTE ReceiveByte()
{
	while((UCSR0A & (1<<RXC0)) == 0);		// ���� ���� ���� ����� ������
	return UDR0;												// ���������� �������� ����

}

// ------------------------------------------------------------------------------

BYTE 	RecvBuffer[USART_RECV_BUFFER_LEN];
BYTE	RecvIndex = 0;


// ------------------------------------------------------------------------------
// ���������� �� ���������� ������
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
		// �������� �� ������ �������
		if (RecvIndex != REQUEST_LEN - 1)
		{
			RecvIndex++;
			return;
		}

		// ��������� CRC8 ���������� ������ 
		if (RecvBuffer[2] != CalcCRC8(&RecvBuffer[3], REQUEST_DATA_LEN))
		{
			RecvIndex = 0;
			return;
		}

		// ��������� ���������� �� ������������ �������
		// �������� ��� ����� �����������
		TIMSK	&= ~(1<<TOIE0);	


		// REQUEST_GET_PORT_IO -------------------------------------------------------
		if (RecvBuffer[0] == REQUEST_GET_PORT_IO)	
			SendPortIO();


		// REQUEST_SET_DISCRETE_STATE ------------------------------------------------
		// RecvBuffer[1] - ����� �����
		// RecvBuffer[2] - ����� �����
		// RecvBuffer[3] - ��������� - ���(0)/��(1)
		if (RecvBuffer[0] == REQUEST_SET_DISCRETE_STATE)	
			SetDiscreteInputState(RecvBuffer[3], RecvBuffer[4], RecvBuffer[5]);



		// REQUEST_GET_EEPROM_PARAM --------------------------------------------------
		if (RecvBuffer[0] == REQUEST_GET_EEPROM_PARAM)	
			SendEepromParam();

		// REQUEST_SET_EEPROM_PARAM --------------------------------------------------
		// RecvBuffer[1] - ����� ������
		// RecvBuffer[2] - ��������
		if (RecvBuffer[0] == REQUEST_SET_EEPROM_PARAM)	
			SetEepromParam(RecvBuffer[3], RecvBuffer[4]);


		// ��������� ���������� �� ������������ �������
		// ������� ��� ����� �����������
		TIMSK	|= (1<<TOIE0);


	
		RecvIndex = 0;
		
	}
	else
		RecvIndex++; 

}

// ------------------------------------------------------------------------------

void SendStr(char* pStr, BYTE StrType)
{	
	while((UCSR0A & (1<<UDRE0)) == 0); 		// ���� ���� ���������� ������� ��������
	UDR0 = MSG_TYPE_STR;  								// �������� ��� ��������� ������

	while((UCSR0A & (1<<UDRE0)) == 0); 		// ���� ���� ���������� ������� ��������
	UDR0 = StrType;  											// �������� ��� �����

	while(*pStr)
	{
		while((UCSR0A & (1<<UDRE0)) == 0);	// ���� ���� ���������� ������� ��������
		UDR0 = *pStr++; 										// �������� ��������� ���� ���������
	}	

	while((UCSR0A & (1<<UDRE0)) == 0); 		// ���� ���� ���������� ������� ��������
	UDR0 = 0x0D;  												// �������� ����-������� ����� ���������



}

// ------------------------------------------------------------------------------

void SendData(BYTE* pBuffer, BYTE BufferLength, BYTE DataType)
{
	while((UCSR0A & (1<<UDRE0)) == 0); 			// ���� ���� ���������� ������� ��������
	UDR0 = MSG_TYPE_DATA;  									// �������� ��� ��������� ������

	while((UCSR0A & (1<<UDRE0)) == 0); 			// ���� ���� ���������� ������� ��������
	UDR0 = BufferLength;  									// �������� ��� ��������� ������
	
	while((UCSR0A & (1<<UDRE0)) == 0); 			// ���� ���� ���������� ������� ��������
	UDR0 = CalcCRC8(pBuffer, BufferLength); // �������� CRC8

	while((UCSR0A & (1<<UDRE0)) == 0); 			// ���� ���� ���������� ������� ��������
	UDR0 = DataType;  											// �������� ��� �����x
	
	while(BufferLength)
	{
		while((UCSR0A & (1<<UDRE0)) == 0);		// ���� ���� ���������� ������� ��������
		UDR0 = *pBuffer++; 										// �������� ��������� ���� ���������

		BufferLength --;
	}

	while((UCSR0A & (1<<UDRE0)) == 0); 			// ���� ���� ���������� ������� ��������
	UDR0 = 0x0D;  													// �������� ����-������� ����� ���������

}


// ------------------------------------------------------------------------------
