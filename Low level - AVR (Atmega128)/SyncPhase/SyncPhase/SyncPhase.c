#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<ctype.h>

#include "SyncPhase.h"
#include "Phase.h"
#include "Signal.h"
#include "Setting.h"
#include "Timer.h"

byte 		ADCInput = 0;													// текущий опрашиваемый вход
byte 		ADCInputs[ADC_INPUT_COUNT] = 	        // опрашиваемые входы АЦП
{
  INPUT_A0,
  INPUT_A1,
};

uint 	ValueADC[ADC_INPUT_COUNT] = {0, 0};			// данные АЦП

byte 	TiristorPhase = PHASE_TYPE_UNDEFINED;		// фаза импульса для тиристора

PHASE Phase[PHASE_TYPE_COUNT];								// три фазы

double 	PeriodPhase360	= 0;									// время периода фазы в сек на 360°

uint	TiristorTickCount = 0;									// к-во имульсов 
																							// для открытия тиристора

byte 	TC0_TickCount = 0;

uint	TC1_TickCount = 0;											// к-во имульсов 
																							// для генратора падающей пилы


// -------------------------------------------------------------------------------
// Окургление входного значения к целому

int RoundVal(double Value)
{
	char str[0x10];
	dtostrf(Value, 6, 0, str);
	return atoi(str);
}


// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// обработчик прерывания от интегрированого АЦП
// его назначение считывать значение с уазаного входа АЦП, 
// и устанвовить следующий вход для опроса

ISR(SIG_ADC)
{
	if (ADCInput >= ADC_INPUT_COUNT)
		return;

	ValueADC[ADCInput] = ADC;

	if(++ADCInput >= ADC_INPUT_COUNT)
		ADCInput = 0;

	ADMUX = ADCInputs[ADCInput]; 	// указывем следующий вход
	
	ADCSRA |= 0x40; 							// запуск нового преобразования АЦП
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// обработчик внешнего прерывания INT0
// по приходу синхроимпульса (миандра) фазы N

ISR(SIG_INTERRUPT0)
{

	// определяем характер пришедшего прерывания: нарастающий или падющий фронт
	// по состоянию регистра EICRA: 3 - нарстающий, 2 - падающий
	switch(EICRA)
	{
		case 3: // пришел нарастающий фронт

			// чтобы достичь точки сопряжения ждем время равное 90° 
			// или при частоте 50Гц это 0,005 сек
			StartTimer0(DIV_TC0_CLK1024, TC0_TickCount);
			
			EIMSK = 0x0; 		// заперещаем прерывание INT0
			
			EIFR  = 0x01; 	// сбрасываем флаг прерывания
			EICRA = 0x02; 	// флаги ICS0 регистра EICRA в состоянии 10, 
											// т.е. далее ждем падающий фронт
			EIMSK = 0x01; 	// разрешаем прерывание INT0
		
			break;

		case 2: // пришел падающий фронт

			EIMSK = 0x0; 		// заперещаем прерывание INT0
			
			EIFR  = 0x01; 	// сбрасываем флаг прерывания
			EICRA = 0x03; 	// флаги ICS0 регистра EICRA в состоянии 11, 
											// т.е. далее ждем нарастающий фронт
			EIMSK = 0x01; 	// разрешаем прерывание INT0

			break;
	}
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// обработчик прерывания при срабатывании таймера счетчика 0
// т.е. дождались точки сопряжения можно запускать 
// генератор падающей пилы (счетчик угла фаз)

ISR(SIG_OVERFLOW0)
{	
	StopTimer0();
	// запускаем генератор падающей пилы, его один так составит 
	// при частоте 50Гц через каждые 
	// 180°/1023 = 0,01/1023 = 0.00000977 сек 
	// его задача считать текущий угол фаз от 0° до 180°
	// активируем счетчик фазы A (это значит что синхронизация по фазе A)
	// разрешаем прерывание на регистр сравнение OCR1A
	InitPhase(PHASE_TYPE_A);

}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// обработчик прерывания от регистра сравнения OCR1C
// его задача тактировать импульсы генератора падающей пилы (счетчика угла фаз)


ISR(SIG_OUTPUT_COMPARE1A)
{	

}

// -------------------------------------------------------------------------------

void OnCompare1C()
{	
	


}

// -------------------------------------------------------------------------------
// Инициализация значений фазы

void InitPhase(byte PhaseType)
{
	if (PhaseType >= PHASE_TYPE_COUNT)
		return;

	// устанавливаем счетчик угла фазы на минимум (от 0° до 180°)
	Phase[PhaseType].DegreeCounter 	= 0;

	// поляность соответсвенно
	switch(PhaseType)
	{
		case PHASE_TYPE_A:
		case PHASE_TYPE_C: Phase[PhaseType].Polarity = POLARITY_NEGATIVE; break;
		case PHASE_TYPE_B: Phase[PhaseType].Polarity = POLARITY_POSITIVE; break;
	}
}

// -------------------------------------------------------------------------------
// задача считать угол фаз от 0° до 180° два раза (полярность - и +)

void SetPhasesDegree()
{
	int pt;

	// если значение счетчика градусов фазы А достигло 60°
	// включаем проверку фазы B
	if (Phase[PHASE_TYPE_A].DegreeCounter == 60)
		InitPhase(PHASE_TYPE_B);
		
	// если значение счетчика градусов фазы А достигло 120°
	// включаем проверку фазы C
	if (Phase[PHASE_TYPE_A].DegreeCounter == 120)
		InitPhase(PHASE_TYPE_C);
			
	// провека фазы А,B и С
	for(pt = 0; pt < PHASE_TYPE_COUNT; pt++)
		UpdatePhaseState(pt); 

	// если все фазы прошли два раза по 180°
	// нужно оставливать генератор падающей пилы
	// и ждать следующего синхроимпульса который его запустит
	//if (EnableStopGenerator())
		//InterruptOCR1(OCR1_TYPE_C, INT_OCR1_DISABLE);

}

// -------------------------------------------------------------------------------

void UpdatePhaseState(byte PhaseType)
{

	// если пройдены два полуперода по 360° - дальше не считаем
	if (Phase[PhaseType].DegreeCounter == 360)
		return;
	
	

	//	if (	Phase[PHASE_TYPE_A].DegreeCounter == 1000 && 	
		//			Phase[PHASE_TYPE_A].Polarity == POLARITY_POSITIVE)
			//SetOutputState(OUTPUT_D19, OUTPUT_ON);
		

		if (Phase[PHASE_TYPE_A].DegreeCounter == 181)
			SetOutputState(OUTPUT_D19, OUTPUT_ON);
		

	// если значение совпало с входным
	//if (Phase[PhaseType].DegreeCounter == ValueADC[0])
		//SetControlOutput(PhaseType, OUTPUT_GROUPE_A, OUTPUT_ON); // на выход лог 1

	// если значение совпало с входным
	//if (Phase[PhaseType].DegreeCounter == ValueADC[1])
		//SetControlOutput(PhaseType, OUTPUT_GROUPE_B, OUTPUT_ON); // на выход лог 1

	// если счетчик градусов дошел до предела т.е до 180°
	if (Phase[PhaseType].DegreeCounter == 180)
	{
		// меняем полярность
		//switch( Phase[PhaseType].Polarity )
		//{
			//case POLARITY_NEGATIVE: Phase[PhaseType].Polarity = POLARITY_POSITIVE; break;
		  //case POLARITY_POSITIVE: Phase[PhaseType].Polarity = POLARITY_NEGATIVE; break;
		//}

		//SetOutputState(OUTPUT_D19, OUTPUT_ON);

		//if (PhaseType == PHASE_TYPE_A)
		//{
			//SetOutputState(OUTPUT_D19, OUTPUT_ON);

		//SetOutputState(OUTPUT_D19, state);

		//if (state == OUTPUT_ON)
			//state = OUTPUT_OFF;
		//else
		//if (state == OUTPUT_OFF)
			//state = OUTPUT_ON;
		//}
	}
	else // если счетчик градусов не дошел до предела т.е до 180°
		Phase[PhaseType].DegreeCounter++; // движемся в направлении к 180°
}

// -------------------------------------------------------------------------------
// Разрешение выключения генератора падающей пилы (счетчика угла фаз)

byte EnableStopGenerator()
{
	// если все три фазы прошли полный период - 360°, то
	// даем разрешение на отановку 
	//if (Phase[PHASE_TYPE_A].EnableCounter == STOP_COUNTER &&
		//	Phase[PHASE_TYPE_B].EnableCounter == STOP_COUNTER &&
			//Phase[PHASE_TYPE_C].EnableCounter == STOP_COUNTER	)
		//return 1;

	return 0;
}

// -------------------------------------------------------------------------------

void SetControlOutput(byte PhaseType, byte GroupeType, byte State)
{
	switch (State)
	{ 
		case OUTPUT_ON: 	
		
		  TiristorPhase = PhaseType; 			
	  	
			int OCR1Type = -1;

			switch(GroupeType)
			{
				case OUTPUT_GROUPE_A: OCR1Type = OCR1_TYPE_A; break;
				case OUTPUT_GROUPE_B: OCR1Type = OCR1_TYPE_B; break;
			}

			if (OCR1Type == -1 )
				break;

			// заносим в регистр сравнения времменную задежку
			// далее установим на выходах лог 1, (начнем формировать миандр)
			// и будем ждать, пока не прийдет прерывание от реистра сравнение
			// чтобы установить на этом же выходе лог 0, (закончим формировать миандр)
			LoadOCR1(OCR1Type, TiristorTickCount); 
			InterruptOCR1(OCR1Type, INT_OCR1_ENABLE); break;
	
      break;

		case OUTPUT_OFF: 	
      TiristorPhase = PHASE_TYPE_UNDEFINED; 
			
			// пришло прерывание от регистра сравнения значит
	    // запрещаем дальнейшее сравнение с регистром
			// далее установим на выходе лог 0
			// завершив тем самым формирование миандра
			switch(GroupeType)
			{
				case OUTPUT_GROUPE_A: InterruptOCR1(OCR1_TYPE_A, INT_OCR1_DISABLE); break;
				case OUTPUT_GROUPE_B: InterruptOCR1(OCR1_TYPE_B, INT_OCR1_DISABLE); break;
			}

      break;
	}

	// уставнавливаем на выходах (в заисмости от полярности) либо лог 0 либо лог 1, 
	// будем надеяться что полярность не успеет поменяться пока 
	// лог 1 будет перетекать в лог 0
	switch( Phase[PhaseType].Polarity )
	{
 		case POLARITY_NEGATIVE: 
			SetOutputState(	Phase[PhaseType].Output1[GroupeType], State); 
			break;

	  case POLARITY_POSITIVE: 
			SetOutputState(	Phase[PhaseType].Output2[GroupeType], State); 
			break; 
	}
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// обработчик прерывания от регистра сравнения OCR1A
// управляет длительностью импульса для откытия управляющего тиристора группы А
// его задача установить на нужном выходе 
// в завсимости от фазы знаение лог 0

ISR(SIG_OUTPUT_COMPARE1A)
{
	if (TiristorPhase >= PHASE_TYPE_COUNT)
		return;

	// устанавливаем на выходе лог 0
	SetControlOutput(TiristorPhase, OUTPUT_GROUPE_A, OUTPUT_OFF);
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// обработчик прерывания от регистра сравнения OCR1B
// управляет длительностью импульса для откытия управляющего тиристора группы B
// его задача установить на нужном выходе 
// в завсимости от фазы знаение лог 0

ISR(SIG_OUTPUT_COMPARE1B)
{
	if (TiristorPhase >= PHASE_TYPE_COUNT)
		return;

	// устанавливаем на выходе лог 0
	SetControlOutput(TiristorPhase, OUTPUT_GROUPE_B, OUTPUT_OFF);
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// главная ф-я отсюда начинается программа

int main(void)
{	
	// --------- загружаем все уставки из энергнонезависемой памяти --------- 

	LoadSettings();

	 // определаем период в секундах от 360° в зависимости от частоты питания
	PeriodPhase360 = 1/50.0;//1/Settings[SETTING_F_POWER].PhysicalVal; 

	// определяем время и количество импульсов для отрытия тиристора 
	double TimeoutTiristor = Settings[SETTING_T_TIRISTOR].PhysicalVal;
	TiristorTickCount = SecondToTick(	TC1, DIV_TC1_CLK, 
																		TimeoutTiristor / 1000000.0 );

	// --------- инициализация таймера счетчика 0 --------- 

	TC0_TickCount = SecondToTick(	TC0, DIV_TC0_CLK1024,  PeriodPhase360 / 4 );

	TC0_TickCount = 0x100 - TC0_TickCount;

	// --------- инициализация таймера счетчика 1 --------- 

	InitTimer1(DIV_TC1_CLK);
	
	// вычисляем количество импульсов для генреатора падающей пилы
	// при частоте 50Гц через каждые, его так составит 
	// 180°/1023 = 0,01/1023 = 0.00000977 сек 
	TC1_TickCount = SecondToTick( TC1, DIV_TC1_CLK, PeriodPhase360 / 2 / 360 );

	// --------- устанавливаем выходы для управления тиристорами --------- 
	
	int pt, OutputA = 0, OutputB = 0;
	for(pt = 0; pt < PHASE_TYPE_COUNT; pt++)
	{
		Phase[pt].Output1[OUTPUT_GROUPE_A] 	= OutputGroupeA[OutputA++];
		Phase[pt].Output2[OUTPUT_GROUPE_A]	= OutputGroupeA[OutputA++];

		Phase[pt].Output1[OUTPUT_GROUPE_B] 	= OutputGroupeB[OutputB++];
		Phase[pt].Output2[OUTPUT_GROUPE_B]	= OutputGroupeB[OutputB++];
	}

	// --------- Настраиваем порты A,C и G как выходы --------- 
	
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRG = 0xFF;

	// --------- инициализация АЦП --------- 
	// указываем что начианм опрашивать с 0-го входа
	// биты REFS и ADLAR обнуляем
	// биты ADPS регистра ADCSRA = 0b00000111 потому, что
	// тактовая частота при кварце 16МГц делиться на 128
	// так как занчение для нормальной работы АЦП
	//	должно находися в переделах от 50 до 200КГц
	// 16000000/128 = 125000 Гц = 125КГц
	ADMUX 		= ADCInputs[ADCInput];
	ADCSRA 		= 0b11001111;

	// --------- разрешение прерывания INT0 (синхроимпульс) --------- 
	// по восходящему фронту это значит что
	// флаги ICS0 регистра EICRA в состоянии 11
	// перывание INT0 может возникнуть при его разрешении, поэтому 
	// отключаем прерывание INT0 путем сброса бита 
	// разрешения прерывания в регистре EIMSK
	// записываем лог 1 во флаг прерываний INT0 регистра EIFR
	// чтобы сбросить флаг прерывания

	EIMSK = 0x0; 		// заперещаем прерывание INT0

	EICRA = 0x03; 	// флаги ICS0 регистра EICRA в состоянии 11
	EIFR  = 0x01; 	// сбрасываем флаг прерывания

	EIMSK = 0x01; 	// разрешаем прерывание INT0

	// --------- общее разрешение всех прерываний --------- 
	sei();

	// USART initialization
	// Communication Parameters: 8 Data, No Parity, 1 Stop  8N1
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	UCSR0A=0x00;
	UCSR0B=0x98;
	UCSR0C=0x86;

	// USART Baud rate: 115200  для кварца 11,059 МГц
	UBRR0H=0x00;
	UBRR0L=0x67; 

	// --------- бесконечный цикл --------- 
	while(1);
		
	return 0;
}

// -------------------------------------------------------------------------------
