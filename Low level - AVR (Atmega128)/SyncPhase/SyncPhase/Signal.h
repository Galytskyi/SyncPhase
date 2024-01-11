
#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include "SyncPhase.h"

// ���������� ������
//
// ���� � 0x1B  ���������� ������
// ���� B 0x18  ���������� �����
// ���� C 0x15  ���������� ������
// ���� D 0x12  ���������� �����
// ���� E 0x03  ���������� �����
// ���� F 0x42  ���������� �����
// ���� G 0x45  ���������� ������


// ��� �������

#define 	SIG_TYPE_UNDEFINED		-1  // ��� ������� �� ���������
#define 	SIG_TYPE_RESERV 			0   // ������
#define 	SIG_TYPE_INPUT_A 			1   // ��� ������� ��. ���������� �������   
#define 	SIG_TYPE_INPUT_D 			2   // ��� ������� ��. ���������� �������
#define 	SIG_TYPE_OUTPUT_A 		3   // ��� ������� ���. ���������� �������
#define 	SIG_TYPE_OUTPUT_D 		4   // ��� ������� ���. ���������� �������
#define 	SIG_TYPE_INTER_A 			5   // ��� ������� ������. ���������� �������
#define 	SIG_TYPE_INTER_D 			6   // ��� ������� ������. ���������� �������

#define 	SIG_TYPE_COUNT 				7


// ���������� ��������

#define   SIG_INPUT_A_COUNT      8  // ���������� ��. ���������� ��������
#define   SIG_INPUT_D_COUNT     24  // ���������� ��. ���������� ��������
#define   SIG_OUTPUT_A_COUNT     0  // ���������� ���. ���������� ��������
#define   SIG_OUTPUT_D_COUNT    21  // ���������� ���. ���������� ��������
#define   SIG_INTER_A_COUNT      0  // ���������� ������. ���������� ��������
#define   SIG_INTER_D_COUNT      0  // ���������� ������. ���������� ��������


// �������� ������������ ��������� ������� 
// ������������ ����� ������

typedef struct _SIG_POS
{
	byte 	Addr;                       // ����� �����
	byte	BitNo;                      // ����� ���� 
 
}SIG_POS;


// ������������ �������, �.� �������� �� �������

extern SIG_POS SigInAanalogPos[SIG_INPUT_A_COUNT];
extern SIG_POS SigInDiscretePos[SIG_INPUT_D_COUNT];
extern SIG_POS SigOutAanalogPos[SIG_OUTPUT_A_COUNT];
extern SIG_POS SigOutDiscretePos[SIG_OUTPUT_D_COUNT];
extern SIG_POS SigIntAnalogPos[SIG_INTER_A_COUNT];
extern SIG_POS SigIntDiscretePos[SIG_INTER_D_COUNT];



#define OUTPUT_UNDEFINED  0xFFFF

// ��. ���������� �������

#define INPUT_A0         0         // ���������� ���� 1
#define INPUT_A1         1         // ���������� ���� 2
#define INPUT_A2         2         // ���������� ���� 3
#define INPUT_A3         3         // ���������� ���� 4
#define INPUT_A4         4         // ���������� ���� 5
#define INPUT_A5         5         // ���������� ���� 6
#define INPUT_A6         6         // ���������� ���� 7
#define INPUT_A7         7         // ���������� ���� 8

// ��. ���������� �������

#define INPUT_D0         0         // ���������� ���� 1
#define INPUT_D1         1         // ���������� ���� 2
#define INPUT_D2         2         // ���������� ���� 3
#define INPUT_D3         3         // ���������� ���� 4
#define INPUT_D4         4         // ���������� ���� 5
#define INPUT_D5         5         // ���������� ���� 6
#define INPUT_D6         6         // ���������� ���� 7
#define INPUT_D7         7         // ���������� ���� 8
#define INPUT_D8         8         // ���������� ���� 9
#define INPUT_D9         9         // ���������� ���� 10
#define INPUT_D10        10        // ���������� ���� 11
#define INPUT_D11        11        // ���������� ���� 12
#define INPUT_D12        12        // ���������� ���� 13
#define INPUT_D13        13        // ���������� ���� 14
#define INPUT_D14        14        // ���������� ���� 15
#define INPUT_D15        15        // ���������� ���� 16
#define INPUT_D16        16        // ���������� ���� 17
#define INPUT_D17        17        // ���������� ���� 18
#define INPUT_D18        18        // ���������� ���� 19
#define INPUT_D19        19        // ���������� ���� 20
#define INPUT_D20        20        // ���������� ���� 21
#define INPUT_D21        21        // ���������� ���� 22
#define INPUT_D22        22        // ���������� ���� 23
#define INPUT_D23        23        // ���������� ���� 24

// ���. ���������� �������

#define OUTPUT_D0         0         // ���������� ����� 1
#define OUTPUT_D1         1         // ���������� ����� 2
#define OUTPUT_D2         2         // ���������� ����� 3
#define OUTPUT_D3         3         // ���������� ����� 4
#define OUTPUT_D4         4         // ���������� ����� 5
#define OUTPUT_D5         5         // ���������� ����� 6
#define OUTPUT_D6         6         // ���������� ����� 7
#define OUTPUT_D7         7         // ���������� ����� 8
#define OUTPUT_D8         8         // ���������� ����� 9
#define OUTPUT_D9         9         // ���������� ����� 10
#define OUTPUT_D10        10        // ���������� ����� 11
#define OUTPUT_D11        11        // ���������� ����� 12
#define OUTPUT_D12        12        // ���������� ����� 13
#define OUTPUT_D13        13        // ���������� ����� 14
#define OUTPUT_D14        14        // ���������� ����� 15
#define OUTPUT_D15        15        // ���������� ����� 16
#define OUTPUT_D16        16        // ���������� ����� 17
#define OUTPUT_D17        17        // ���������� ����� 18
#define OUTPUT_D18        18        // ���������� ����� 19
#define OUTPUT_D19        19        // ���������� ����� 20
#define OUTPUT_D20        20        // ���������� ����� 21


// ��� ������������� ��������� ������
typedef enum{INPUT_OFF, INPUT_ON} INPUT_STATE; 

// ��� ������������� ��������� �������
typedef enum{OUTPUT_OFF, OUTPUT_ON} OUTPUT_STATE; 

// �������� ��������� �����
INPUT_STATE GetInputState(byte Input); 

// ���������� �������� ������
void SetOutputState(byte Output, byte State); 

// ���������� ������ ����� A � B, 
#define 	OUTPUT_GROUPE_A					0
#define 	OUTPUT_GROUPE_B					1

#define 	OUTPUT_GROUPE_COUNT			2

// 6 ������� ��� ��� ���� �� ��� ( ��� ���������� "+" � "-") ������

extern 		byte OutputGroupeA[6];
extern 		byte OutputGroupeB[6];

#endif /* _SIGNAL_H_ */
