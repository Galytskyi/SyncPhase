
#ifndef _TIMER_H_
#define _TIMER_H_

#include "SyncPhase.h"

// Перечисление таймеров счетчиков

#define 			TC0 							0
#define 			TC1 							1
#define 			TC2 							2
#define 			TC3 							3

#define 			TC_COUNT					4


uint SecondToTick(byte Timer, byte DivisorType, double Second);

// --------- Таймер Счетчик 0 ---------

// Делители таймера счетчика 0

#define 			DIV_TC0_CLK 			0
#define 			DIV_TC0_CLK8			1
#define 			DIV_TC0_CLK32			2
#define 			DIV_TC0_CLK64			3
#define 			DIV_TC0_CLK128		4
#define 			DIV_TC0_CLK256		5
#define 			DIV_TC0_CLK1024		6

#define 			DIV_TC0_COUNT			7

// запуск и остновка таймера счетчика 0
// DivisorType - делитель таймера на 1, 8, 32, 64, 128, 256 или 1024 
// Second - время в секундах задержки таймера, 
void StartTimer0(byte DivisorType, byte TickCount);
void StopTimer0();

// --------- Таймер Счетчик 1 ---------

// Делители таймера счетчика 1

#define 			DIV_TC1_CLK 			0
#define 			DIV_TC1_CLK8			1
#define 			DIV_TC1_CLK64			2
#define 			DIV_TC1_CLK256		3
#define 			DIV_TC1_CLK1024		4

#define 			DIV_TC1_COUNT			5

// Регистры сравнения таймера счетчика 1

#define 			OCR1_TYPE_A 			0
#define 			OCR1_TYPE_B				1
#define 			OCR1_TYPE_C				2
#define 			OCR3_TYPE_A 			3
#define 			OCR3_TYPE_B				4
#define 			OCR3_TYPE_C				5

#define 			OCR_TYPE_COUNT		6

#define 			INT_OCR_DISABLE	0
#define 			INT_OCR_ENABLE		1

// инициализация таймера счетчика 1
// DivisorType - делитель таймера на 1, 8, 64, 256 или 1024 
void InitTimer1(byte DivisorType);

// разрешение или запрет прерывания от регистра сравнения
void InterruptOCR1(byte OCRType, byte Enable);

// загрузить значение в регистр сравнения
void LoadOCR1(byte OCRType, uint TickCount);

// остновка таймера счетчика 1
void StopTimer1();


#endif /* _TIMER_H_ */
