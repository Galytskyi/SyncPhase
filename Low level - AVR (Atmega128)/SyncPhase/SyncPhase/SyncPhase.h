
#ifndef _SYNCPHASE_H_
#define _SYNCPHASE_H_

typedef unsigned char byte;
typedef unsigned int 	uint;

#define 			FreqQuartz				16000000.0		 	// ������� ������

#define 			ADC_INPUT_COUNT 	2							// ���������� ������ ������������ ���

#define				LOW_ADC 					0							// ������ �������� ���
#define				HIGH_ADC 					1023					// ������� �������� ���

// ���������� �������� �� ������ �����
int 					RoundVal(double Value);

// ��� �������� ����� ���������� �� ���������� 8-�� ��������� ������ ������� 0
// ��� ������ � �������������� �� ����������� 16-�� ��������� ������ ������� 1
// ������������ �������� ��������� OCR1A, OCR1B � OCR1C
// �������� ��������� OCR1A, OCR1B - ��� ��������� ��� 0 
// �� ������� ����������� ����������
// ������� ��������� OCR1C - ��� ������������ ��������� 
// ���������� �������� ���� (������� ���� ���)	

// ����������� ���������� 
void 					OnCompare1C();

void 					InitPhase(byte PhaseType);
void 					SetPhasesDegree(); 
void 					UpdatePhaseState(byte PhaseType);

byte 					EnableStopGenerator();

void 					SetControlOutput(byte PhaseType, byte GroupeType, byte State);


#endif /* _SYNCPHASE_H_ */


