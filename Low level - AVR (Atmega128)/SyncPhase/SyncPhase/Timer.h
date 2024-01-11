
#ifndef _TIMER_H_
#define _TIMER_H_

#include "SyncPhase.h"

// ������������ �������� ���������

#define 			TC0 							0
#define 			TC1 							1
#define 			TC2 							2
#define 			TC3 							3

#define 			TC_COUNT					4


uint SecondToTick(byte Timer, byte DivisorType, double Second);

// --------- ������ ������� 0 ---------

// �������� ������� �������� 0

#define 			DIV_TC0_CLK 			0
#define 			DIV_TC0_CLK8			1
#define 			DIV_TC0_CLK32			2
#define 			DIV_TC0_CLK64			3
#define 			DIV_TC0_CLK128		4
#define 			DIV_TC0_CLK256		5
#define 			DIV_TC0_CLK1024		6

#define 			DIV_TC0_COUNT			7

// ������ � �������� ������� �������� 0
// DivisorType - �������� ������� �� 1, 8, 32, 64, 128, 256 ��� 1024 
// Second - ����� � �������� �������� �������, 
void StartTimer0(byte DivisorType, byte TickCount);
void StopTimer0();

// --------- ������ ������� 1 ---------

// �������� ������� �������� 1

#define 			DIV_TC1_CLK 			0
#define 			DIV_TC1_CLK8			1
#define 			DIV_TC1_CLK64			2
#define 			DIV_TC1_CLK256		3
#define 			DIV_TC1_CLK1024		4

#define 			DIV_TC1_COUNT			5

// �������� ��������� ������� �������� 1

#define 			OCR1_TYPE_A 			0
#define 			OCR1_TYPE_B				1
#define 			OCR1_TYPE_C				2
#define 			OCR3_TYPE_A 			3
#define 			OCR3_TYPE_B				4
#define 			OCR3_TYPE_C				5

#define 			OCR_TYPE_COUNT		6

#define 			INT_OCR_DISABLE	0
#define 			INT_OCR_ENABLE		1

// ������������� ������� �������� 1
// DivisorType - �������� ������� �� 1, 8, 64, 256 ��� 1024 
void InitTimer1(byte DivisorType);

// ���������� ��� ������ ���������� �� �������� ���������
void InterruptOCR1(byte OCRType, byte Enable);

// ��������� �������� � ������� ���������
void LoadOCR1(byte OCRType, uint TickCount);

// �������� ������� �������� 1
void StopTimer1();


#endif /* _TIMER_H_ */
