
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "SyncPhase.h"

// Назначение портов
//
// Порт А 0x1B  дискретные выходы
// Порт B 0x18  дискретные входы
// Порт C 0x15  дискретные выходы
// Порт D 0x12  дискретные входы
// Порт E 0x03  дискретные входы
// Порт F 0x42  аналоговые входы
// Порт G 0x45  дискретные выходы


// тип сигнала

#define 	SIG_TYPE_UNDEFINED		-1  // тип сигнала не определен
#define 	SIG_TYPE_RESERV 			0   // резерв
#define 	SIG_TYPE_INPUT_A 			1   // тип сигнала вх. аналоговые сигналы   
#define 	SIG_TYPE_INPUT_D 			2   // тип сигнала вх. дискретные сигналы
#define 	SIG_TYPE_OUTPUT_A 		3   // тип сигнала вых. аналоговые сигналы
#define 	SIG_TYPE_OUTPUT_D 		4   // тип сигнала вых. дискретные сигналы
#define 	SIG_TYPE_INTER_A 			5   // тип сигнала промеж. аналоговые сигналы
#define 	SIG_TYPE_INTER_D 			6   // тип сигнала промеж. дискретные сигналы

#define 	SIG_TYPE_COUNT 				7


// количество сигналов

#define   SIG_INPUT_A_COUNT      8  // количество вх. аналоговых сигналов
#define   SIG_INPUT_D_COUNT     24  // количество вх. дискретных сигналов
#define   SIG_OUTPUT_A_COUNT     0  // количество вых. аналоговых сигналов
#define   SIG_OUTPUT_D_COUNT    21  // количество вых. дискретных сигналов
#define   SIG_INTER_A_COUNT      0  // количество промеж. аналоговых сигналов
#define   SIG_INTER_D_COUNT      0  // количество промеж. дискретных сигналов


// стуктура определяощая положение сигнала 
// относительно ножки выхода

typedef struct _SIG_POS
{
	byte 	Addr;                       // Адрес порта
	byte	BitNo;                      // Номер бита 
 
}SIG_POS;


// Расположение выводов, т.е разбивка по потртам

extern SIG_POS SigInAanalogPos[SIG_INPUT_A_COUNT];
extern SIG_POS SigInDiscretePos[SIG_INPUT_D_COUNT];
extern SIG_POS SigOutAanalogPos[SIG_OUTPUT_A_COUNT];
extern SIG_POS SigOutDiscretePos[SIG_OUTPUT_D_COUNT];
extern SIG_POS SigIntAnalogPos[SIG_INTER_A_COUNT];
extern SIG_POS SigIntDiscretePos[SIG_INTER_D_COUNT];



#define OUTPUT_UNDEFINED  0xFFFF

// вх. аналоговые сигналы

#define INPUT_A0         0         // Аналоговый вход 1
#define INPUT_A1         1         // Аналоговый вход 2
#define INPUT_A2         2         // Аналоговый вход 3
#define INPUT_A3         3         // Аналоговый вход 4
#define INPUT_A4         4         // Аналоговый вход 5
#define INPUT_A5         5         // Аналоговый вход 6
#define INPUT_A6         6         // Аналоговый вход 7
#define INPUT_A7         7         // Аналоговый вход 8

// вх. дискретные сигналы

#define INPUT_D0         0         // Дискретный вход 1
#define INPUT_D1         1         // Дискретный вход 2
#define INPUT_D2         2         // Дискретный вход 3
#define INPUT_D3         3         // Дискретный вход 4
#define INPUT_D4         4         // Дискретный вход 5
#define INPUT_D5         5         // Дискретный вход 6
#define INPUT_D6         6         // Дискретный вход 7
#define INPUT_D7         7         // Дискретный вход 8
#define INPUT_D8         8         // Дискретный вход 9
#define INPUT_D9         9         // Дискретный вход 10
#define INPUT_D10        10        // Дискретный вход 11
#define INPUT_D11        11        // Дискретный вход 12
#define INPUT_D12        12        // Дискретный вход 13
#define INPUT_D13        13        // Дискретный вход 14
#define INPUT_D14        14        // Дискретный вход 15
#define INPUT_D15        15        // Дискретный вход 16
#define INPUT_D16        16        // Дискретный вход 17
#define INPUT_D17        17        // Дискретный вход 18
#define INPUT_D18        18        // Дискретный вход 19
#define INPUT_D19        19        // Дискретный вход 20
#define INPUT_D20        20        // Дискретный вход 21
#define INPUT_D21        21        // Дискретный вход 22
#define INPUT_D22        22        // Дискретный вход 23
#define INPUT_D23        23        // Дискретный вход 24

// вых. дискретные сигналы

#define OUTPUT_D0         0         // Дискретный выход 1
#define OUTPUT_D1         1         // Дискретный выход 2
#define OUTPUT_D2         2         // Дискретный выход 3
#define OUTPUT_D3         3         // Дискретный выход 4
#define OUTPUT_D4         4         // Дискретный выход 5
#define OUTPUT_D5         5         // Дискретный выход 6
#define OUTPUT_D6         6         // Дискретный выход 7
#define OUTPUT_D7         7         // Дискретный выход 8
#define OUTPUT_D8         8         // Дискретный выход 9
#define OUTPUT_D9         9         // Дискретный выход 10
#define OUTPUT_D10        10        // Дискретный выход 11
#define OUTPUT_D11        11        // Дискретный выход 12
#define OUTPUT_D12        12        // Дискретный выход 13
#define OUTPUT_D13        13        // Дискретный выход 14
#define OUTPUT_D14        14        // Дискретный выход 15
#define OUTPUT_D15        15        // Дискретный выход 16
#define OUTPUT_D16        16        // Дискретный выход 17
#define OUTPUT_D17        17        // Дискретный выход 18
#define OUTPUT_D18        18        // Дискретный выход 19
#define OUTPUT_D19        19        // Дискретный выход 20
#define OUTPUT_D20        20        // Дискретный выход 21


// тип определяющщий состояние входов
typedef enum{INPUT_OFF, INPUT_ON} INPUT_STATE; 

// тип определяющщий состояние выходов
typedef enum{OUTPUT_OFF, OUTPUT_ON} OUTPUT_STATE; 

// получить состояние входа
INPUT_STATE GetInputState(byte Input); 

// установить значение выхода
void SetOutputState(byte Output, byte State); 

// Дискретные выходы групп A и B, 
#define 	OUTPUT_GROUPE_A					0
#define 	OUTPUT_GROUPE_B					1

#define 	OUTPUT_GROUPE_COUNT			2

// 6 выходов это три фазы по два ( для полярности "+" и "-") выхода

extern 		byte OutputGroupeA[6];
extern 		byte OutputGroupeB[6];

#endif /* _SIGNAL_H_ */
