#include <avr/io.h>
#include <avr/interrupt.h>
#include "Simulator.h"
#include "USART.h"
#include "DiagPorts.h"


// Диагностируемые порты
// ------------------------------------------------------------------------------


BYTE AddressPort[DIAG_DISCRETE_PORT_COUNT][DIAG_DISCRETE_PORT_TYPE_COUNT] =
{
	{
		0x3A, // Состояние "вход/выход" Порта A
		0x3B, // Порт A
		0x39, // Состояние "Ножки" Порта A
	},
	{
		0x37, // Состояние "вход/выход" Порта B
		0x38, // Порт B
		0x36, // Состояние "Ножки" Порта B
	},
	{
		0x34, // Состояние "вход/выход" Порта C
		0x35, // Порт C
		0x33, // Состояние "Ножки" Порта C
	},
	{
		0x31, // Состояние "вход/выход" Порта D
		0x32, // Порт D
		0x30, // Состояние "Ножки" Порта D
	},
	{
		0x22, // Состояние "вход/выход" Порта E
		0x23, // Порт E
		0x21, // Состояние "Ножки" Порта E
	},
	{
		0x61, // Состояние "вход/выход" Порта F
		0x62, // Порт F
		0x20, // Состояние "Ножки" Порта F
	},
	{
		0x64, // Состояние "вход/выход" Порта G
		0x65, // Порт G
		0x63, // Состояние "Ножки" Порта G
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
		
	// сбрасываем все флаги прерываний если они были установлены
	TIFR &= ~(1<<OCF0);
	TIFR &= ~(1<<TOV0);

	// Разрешаем прерывание по переполнению таймера
	TIMSK	|= (1<<TOIE0);

	// 
	TCNT0 = 0x0;


	// FOC0	WGM00	COM01	COM00	WGM01	CS02	CS01	CS00 - TCCR0
	// WGM00 WGM01  = 00 - нормальный режим работы
	// COM01 COM00	 = 00	- Таймер/счетчик отсоединен от выходного вывода OC0
	// CS02 CS01 CS00 = 111 - СК / 1024
	
	TCCR0 = 0x07;
}


// ------------------------------------------------------------------------------

void InitTimer2()
{
		
	// сбрасываем все флаги прерываний если они были установлены
	TIFR &= ~(1<<OCF2);
	TIFR &= ~(1<<TOV2);


	// Разрешаем прерывание по переполнению таймера
	TIMSK	|= (1<<TOIE2);

	// 
	TCNT2 = 0x0;


	// FOC2	WGM20	COM21	COM20	WGM21	CS22	CS21	CS20 - TCCR2
	// WGM20 WGM21  = 00 - нормальный режим работы
	// COM21 COM20	 = 00	- Таймер/счетчик отсоединен от выходного вывода OC0
	// CS22 CS21 CS20 = 111 - СК / 1024
	
	TCCR2 = 0x04;
}

// ------------------------------------------------------------------------------

void InitADC()
{
		
	// биты REFS1 REFS0 = 00 - AREF, внутренний VИОН отключен
	// ADLAR = 0 - Бит управления представлением результата преобразования 
	// MUX == 00000 Биты выбора аналогового канала 

	ADMUX = 0x00;
	
	// биты ADPS регистра ADCSRA = 0b00000111 потому, что
	// тактовая частота при кварце 16МГц делиться на 128
	// так как занчение для нормальной работы АЦП
	//	должно находися в переделах от 50 до 200КГц
	// 16000000/128 = 125000 Гц = 125КГц

	// ADEN = 1 - Разрешение работы АЦП 
	// ADSC = 1 - Запуск преобразования АЦП 
	// ADFR = 0 - Выбор режима автоматического перезапуска АЦП 
	// ADIF = 0 - Флаг прерывания АЦП 
	// ADIE	= 1 - Разрешение прерывания АЦП 
	// ADPS	=	111 - Биты управления предделителем АЦП

	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

}


// ------------------------------------------------------------------------------
// Пререывание по таймеру 1

ISR (TIMER0_OVF_vect)	
{
	
	// когда количество тиков достигает пол секунды
	if (Tick0Count == HALF_SECOND_TICK_COUNT)
		GetDiscreteInputsState();

	// когда количество тиков достигает одной секунды
	if (Tick0Count == ONE_SECOND_TICK_COUNT)
	{
		SendData(DiagArray, DIAG_ARRAY_LEN, MSG_TYPE_DATA_PORT_STATE);
		
		PORTG ^= 0x10;
		Tick0Count = 0;
	}


	Tick0Count++;
}


// ------------------------------------------------------------------------------
// Прерывание по таймеру 2

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
// Диагонстика аналоговых входов

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
// Диагонстика дискретных входов/выходов

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
			if (*pAddressPortDDR & (0x1 << Input)) 	// Если ножка назначена как "Выход"
			{
				if (*pAddressPortDATA & (0x1 << Input))	
					State |= (1<<Input);
				else
					State &= ~(1<<Input);
			}
			else																		// Если ножка назначена как "Вход"
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
// Уставнока занчения Да/Нет на указаном выходе

void SetDiscreteInputState(BYTE Port, BYTE Input, BOOL State)
{
	BYTE* pAddressPort = AddressPort[Port][DIAG_DISCRETE_PORT_TYPE_DATA];

	if(!State)
		*pAddressPort &= ~(1<<Input);
	else
		*pAddressPort |= (1<<Input);


}

// ------------------------------------------------------------------------------
// Отправление текужего стояния портов ввод вывода

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

