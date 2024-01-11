#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.h"

int DivisorValueTC0[DIV_TC0_COUNT] = {1, 8, 32, 64, 128, 256, 1024};
int DivisorValueTC1[DIV_TC1_COUNT] = {1, 8, 64, 256, 1024};

// -------------------------------------------------------------------------------
uint SecondToTick(byte Timer, byte DivisorType, double Second)
{
	if (Timer >= TC_COUNT)
		return 0;

		// оределяем количество секунд на один импульс от таймера 
	double SecendInTick = 0; 

	switch(Timer)
	{
		case TC0: 
		case TC2: SecendInTick = DivisorValueTC0[DivisorType] / FreqQuartz; break;
		case TC1:
		case TC3: SecendInTick = DivisorValueTC1[DivisorType] / FreqQuartz; break;
	}

	if (!SecendInTick)
		return 0;

	// оределяем количество импульзов в еденицу времени
	return RoundVal ( Second / SecendInTick );
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

void StartTimer0(byte DivisorType, byte TickCount)
{
	TIFR = 0;
	TIMSK |= _BV(TOIE0);
	TCNT0 = TickCount;
	TCCR0 = DivisorType + 1;
}

// -------------------------------------------------------------------------------

void StopTimer0()
{
	TCCR0 = 0x0;
	TIMSK &= ~_BV(TOIE0);
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

void InitTimer1(byte DivisorType)
{
	if (DivisorType >= DIV_TC1_COUNT)
		return;

	// устанавливаем управлющие регистры
	TCCR1A = 0x0;
	TCCR1B = DivisorType + 1;
	TCCR1C = 0x0;

	// сбрасываем все флаги прерываний если они были установлены
	TIFR = 0;
}

// -------------------------------------------------------------------------------

void InterruptOCR1(byte OCRType, byte Enable)
{
	if (OCRType >= OCR_TYPE_COUNT)
		return;

	switch(Enable)
	{

		case INT_OCR1_ENABLE:
			switch(OCRType)
			{
				case OCR1_TYPE_A:	TIMSK 	|= _BV(OCIE1A); break;
				case OCR1_TYPE_B:	TIMSK 	|= _BV(OCIE1B); break;
				case OCR1_TYPE_C:	ETIMSK 	|= _BV(OCIE1C); break;
			}
			break;

		case INT_OCR1_DISABLE:
			switch(OCRType)
			{
				case OCR1_TYPE_A:	TIMSK 	&= ~_BV(OCIE1A); break;
				case OCR1_TYPE_B:	TIMSK 	&= ~_BV(OCIE1B); break;
				case OCR1_TYPE_C:	ETIMSK 	&= ~_BV(OCIE1C); break;
			}
			break;	 
	}
}

// -------------------------------------------------------------------------------

void LoadOCR1(byte OCRType, uint TickCount)
{
	if (OCRType >= OCR_TYPE_COUNT)
		return;

	// запоминаме состояние регистра SREG
	// запрещаем прерывания
	// уставливаеи значение таймера
	// востаналиваем состояние регистра SREG
	//byte sreg = SREG;
	//cli();

	switch(OCRType)
	{
		case OCR1_TYPE_A: OCR1A = TCNT1 + TickCount; break;
		case OCR1_TYPE_B: OCR1B = TCNT1 + TickCount; break;
		case OCR1_TYPE_C: OCR1C = TCNT1 + TickCount; break;
	}

	//SREG = sreg;
}

// -------------------------------------------------------------------------------

void StopTimer1()
{
	TCCR1B = 0x0;
}

// -------------------------------------------------------------------------------
