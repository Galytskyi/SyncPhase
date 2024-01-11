#include <avr/io.h>
#include <avr/interrupt.h>
#include "Simulator.h"
#include "USART.h"
#include "DiagPorts.h"


// ��������������� �����
// ------------------------------------------------------------------------------


BYTE AddressPort[DIAG_DISCRETE_PORT_COUNT][DIAG_DISCRETE_PORT_TYPE_COUNT] =
{
	{
		0x3A, // ��������� "����/�����" ����� A
		0x3B, // ���� A
		0x39, // ��������� "�����" ����� A
	},
	{
		0x37, // ��������� "����/�����" ����� B
		0x38, // ���� B
		0x36, // ��������� "�����" ����� B
	},
	{
		0x34, // ��������� "����/�����" ����� C
		0x35, // ���� C
		0x33, // ��������� "�����" ����� C
	},
	{
		0x31, // ��������� "����/�����" ����� D
		0x32, // ���� D
		0x30, // ��������� "�����" ����� D
	},
	{
		0x22, // ��������� "����/�����" ����� E
		0x23, // ���� E
		0x21, // ��������� "�����" ����� E
	},
	{
		0x61, // ��������� "����/�����" ����� F
		0x62, // ���� F
		0x20, // ��������� "�����" ����� F
	},
	{
		0x64, // ��������� "����/�����" ����� G
		0x65, // ���� G
		0x63, // ��������� "�����" ����� G
	}
};

// ------------------------------------------------------------------------------

BYTE DiagArray[DIAG_ARRAY_LEN];

BYTE* pDiagDiscreteArray 	= (BYTE*) &DiagArray[0];
WORD* pDiagAnalogArray 		= (WORD*) &DiagArray[DIAG_DISCRETE_PORTS_ARRAY_LEN];

// ------------------------------------------------------------------------------

BYTE Tick0Count = 0;

// ------------------------------------------------------------------------------

void StartDiagPorts()
{	
	InitTimer0();
	//InitTimer2();
	InitADC();
}

// ------------------------------------------------------------------------------

void InitTimer0()
{
		
	// ���������� ��� ����� ���������� ���� ��� ���� �����������
	TIFR &= ~(1<<OCF0);
	TIFR &= ~(1<<TOV0);

	// ��������� ���������� �� ������������ �������
	TIMSK	|= (1<<TOIE0);

	// 
	TCNT0 = 0x0;


	// FOC0	WGM00	COM01	COM00	WGM01	CS02	CS01	CS00 - TCCR0
	// WGM00 WGM01  = 00 - ���������� ����� ������
	// COM01 COM00	 = 00	- ������/������� ���������� �� ��������� ������ OC0
	// CS02 CS01 CS00 = 111 - �� / 1024
	
	TCCR0 = 0x07;
}


// ------------------------------------------------------------------------------

void InitTimer2()
{
		
	// ���������� ��� ����� ���������� ���� ��� ���� �����������
	TIFR &= ~(1<<OCF2);
	TIFR &= ~(1<<TOV2);


	// ��������� ���������� �� ������������ �������
	TIMSK	|= (1<<TOIE2);

	// 
	TCNT2 = 0x0;


	// FOC2	WGM20	COM21	COM20	WGM21	CS22	CS21	CS20 - TCCR2
	// WGM20 WGM21  = 00 - ���������� ����� ������
	// COM21 COM20	 = 00	- ������/������� ���������� �� ��������� ������ OC0
	// CS22 CS21 CS20 = 111 - �� / 1024
	
	TCCR2 = 0x04;
}

// ------------------------------------------------------------------------------

void InitADC()
{
		
	// ���� REFS1 REFS0 = 00 - AREF, ���������� V��� ��������
	// ADLAR = 0 - ��� ���������� �������������� ���������� �������������� 
	// MUX == 00000 ���� ������ ����������� ������ 

	ADMUX = 0x00;
	
	// ���� ADPS �������� ADCSRA = 0b00000111 ������, ���
	// �������� ������� ��� ������ 16��� �������� �� 128
	// ��� ��� �������� ��� ���������� ������ ���
	//	������ �������� � ��������� �� 50 �� 200���
	// 16000000/128 = 125000 �� = 125���

	// ADEN = 1 - ���������� ������ ��� 
	// ADSC = 1 - ������ �������������� ��� 
	// ADFR = 0 - ����� ������ ��������������� ����������� ��� 
	// ADIF = 0 - ���� ���������� ��� 
	// ADIE	= 1 - ���������� ���������� ��� 
	// ADPS	=	111 - ���� ���������� ������������� ���

	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

}


// ------------------------------------------------------------------------------
// ����������� �� ������� 1

ISR (TIMER0_OVF_vect)	
{
	
	// ����� ���������� ����� ��������� ��� �������
	if (Tick0Count == HALF_SECOND_TICK_COUNT)
		GetDiscreteInputsState();

	// ����� ���������� ����� ��������� ����� �������
	if (Tick0Count == ONE_SECOND_TICK_COUNT)
	{
		SendData(DiagArray, DIAG_ARRAY_LEN, MSG_TYPE_DATA_PORT_STATE);
		
		PORTG ^= 0x10;
		Tick0Count = 0;
	}


	Tick0Count++;
}


// ------------------------------------------------------------------------------
// ���������� �� ������� 2

BYTE Tick2Count = 0;

ISR (TIMER2_OVF_vect)	
{
	if (Tick2Count == 245)
	{
		PORTG ^= 0x08;
		Tick2Count = 0;
	}

	Tick2Count ++;

}


// ------------------------------------------------------------------------------
// ����������� ���������� ������

BYTE InputADC = 0;

ISR(SIG_ADC)
{
	pDiagAnalogArray[InputADC++] = ADC;

	if(InputADC == INPUT_ADC_COUNT)
		InputADC = 0;

	ADMUX = InputADC;

	ADCSRA |= (1<<ADSC);
}


// ------------------------------------------------------------------------------
// ����������� ���������� ������/�������

void GetDiscreteInputsState()
{
	BYTE Port = 0;
	
	while(Port != DIAG_DISCRETE_PORT_COUNT)
	{
		BYTE* pAddressPortDDR 	= AddressPort[ Port ][ DIAG_DISCRETE_PORT_TYPE_IO ];
		BYTE* pAddressPortDATA 	= AddressPort[ Port ][ DIAG_DISCRETE_PORT_TYPE_DATA ];
		BYTE* pAddressPortPIN 	= AddressPort[ Port ][ DIAG_DISCRETE_PORT_TYPE_PIN ];
	
		BOOL State = 0;
		BYTE Input = 0;	
		while(Input != DIAG_DISCRETE_INPUT_COUNT)
		{
			if (*pAddressPortDDR & (0x1 << Input)) 	// ���� ����� ��������� ��� "�����"
			{
				if (*pAddressPortDATA & (0x1 << Input))	
					State |= (1<<Input);
				else
					State &= ~(1<<Input);
			}
			else																		// ���� ����� ��������� ��� "����"
			{
				if (*pAddressPortPIN & (0x1 << Input))	
					State |= (1<<Input);
				else
					State &= ~(1<<Input);
			}

			Input++;
		}

		pDiagDiscreteArray[Port] = State;
	
		Port++;
	}

}

// ------------------------------------------------------------------------------
// ��������� �������� ��/��� �� �������� ������

void SetDiscreteInputState(BYTE Port, BYTE Input, BOOL State)
{
	BYTE* pAddressPort = AddressPort[Port][DIAG_DISCRETE_PORT_TYPE_DATA];

	if(!State)
		*pAddressPort &= ~(1<<Input);
	else
		*pAddressPort |= (1<<Input);


}

// ------------------------------------------------------------------------------
// ����������� �������� ������� ������ ���� ������

void SendPortIO()
{

	BYTE PortIO[DIAG_DISCRETE_PORT_COUNT];
	BYTE Port = 0;
	
	while(Port != DIAG_DISCRETE_PORT_COUNT)
	{
		BYTE* pAddressPortDDR  = AddressPort[ Port ][ DIAG_DISCRETE_PORT_TYPE_IO ];
	
		PortIO[Port] = *pAddressPortDDR ;
	
		Port++;
	}

	SendData(PortIO, DIAG_DISCRETE_PORT_COUNT, MSG_TYPE_DATA_PORT_IO);
}

// ------------------------------------------------------------------------------

