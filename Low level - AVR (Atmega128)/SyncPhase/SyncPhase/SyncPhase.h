
#ifndef _SYNCPHASE_H_
#define _SYNCPHASE_H_

typedef unsigned char byte;
typedef unsigned int 	uint;

#define 			FreqQuartz				16000000.0		 	// частота кварца

#define 			ADC_INPUT_COUNT 	2							// количество входов опрашиваемых АЦП

#define				LOW_ADC 					0							// нижнее значение АЦП
#define				HIGH_ADC 					1023					// верхнее значение АЦП

// округление значения до целого числа
int 					RoundVal(double Value);

// для ожидания точки сопряжения мы задейсвуем 8-ми разрядный таймер счетчик 0
// для работы с синхроимульсом мы задействуем 16-ти разрядный таймер счетчик 1
// испопользуем регистры сравнений OCR1A, OCR1B и OCR1C
// регистры сравнений OCR1A, OCR1B - для установки лог 0 
// на выходах управляющих тиристоров
// регистр сравнения OCR1C - для формирования импульсов 
// генератора падающей пилы (счетчик угла фаз)	

// обработчики прерываний 
void 					OnCompare1C();

void 					InitPhase(byte PhaseType);
void 					SetPhasesDegree(); 
void 					UpdatePhaseState(byte PhaseType);

byte 					EnableStopGenerator();

void 					SetControlOutput(byte PhaseType, byte GroupeType, byte State);


#endif /* _SYNCPHASE_H_ */


