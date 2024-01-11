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

byte 		ADCInput = 0;													// ������� ������������ ����
byte 		ADCInputs[ADC_INPUT_COUNT] = 	        // ������������ ����� ���
{
  INPUT_A0,
  INPUT_A1,
};

uint 	ValueADC[ADC_INPUT_COUNT] = {0, 0};			// ������ ���

byte 	TiristorPhase = PHASE_TYPE_UNDEFINED;		// ���� �������� ��� ���������

PHASE Phase[PHASE_TYPE_COUNT];								// ��� ����

double 	PeriodPhase360	= 0;									// ����� ������� ���� � ��� �� 360�

uint	TiristorTickCount = 0;									// �-�� �������� 
																							// ��� �������� ���������

byte 	TC0_TickCount = 0;

uint	TC1_TickCount = 0;											// �-�� �������� 
																							// ��� ��������� �������� ����


// -------------------------------------------------------------------------------
// ���������� �������� �������� � ������

int RoundVal(double Value)
{
	char str[0x10];
	dtostrf(Value, 6, 0, str);
	return atoi(str);
}


// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// ���������� ���������� �� ��������������� ���
// ��� ���������� ��������� �������� � �������� ����� ���, 
// � ����������� ��������� ���� ��� ������

ISR(SIG_ADC)
{
	if (ADCInput >= ADC_INPUT_COUNT)
		return;

	ValueADC[ADCInput] = ADC;

	if(++ADCInput >= ADC_INPUT_COUNT)
		ADCInput = 0;

	ADMUX = ADCInputs[ADCInput]; 	// �������� ��������� ����
	
	ADCSRA |= 0x40; 							// ������ ������ �������������� ���
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// ���������� �������� ���������� INT0
// �� ������� �������������� (�������) ���� N

ISR(SIG_INTERRUPT0)
{

	// ���������� �������� ���������� ����������: ����������� ��� ������� �����
	// �� ��������� �������� EICRA: 3 - ����������, 2 - ��������
	switch(EICRA)
	{
		case 3: // ������ ����������� �����

			// ����� ������� ����� ���������� ���� ����� ������ 90� 
			// ��� ��� ������� 50�� ��� 0,005 ���
			StartTimer0(DIV_TC0_CLK1024, TC0_TickCount);
			
			EIMSK = 0x0; 		// ���������� ���������� INT0
			
			EIFR  = 0x01; 	// ���������� ���� ����������
			EICRA = 0x02; 	// ����� ICS0 �������� EICRA � ��������� 10, 
											// �.�. ����� ���� �������� �����
			EIMSK = 0x01; 	// ��������� ���������� INT0
		
			break;

		case 2: // ������ �������� �����

			EIMSK = 0x0; 		// ���������� ���������� INT0
			
			EIFR  = 0x01; 	// ���������� ���� ����������
			EICRA = 0x03; 	// ����� ICS0 �������� EICRA � ��������� 11, 
											// �.�. ����� ���� ����������� �����
			EIMSK = 0x01; 	// ��������� ���������� INT0

			break;
	}
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// ���������� ���������� ��� ������������ ������� �������� 0
// �.�. ��������� ����� ���������� ����� ��������� 
// ��������� �������� ���� (������� ���� ���)

ISR(SIG_OVERFLOW0)
{	
	StopTimer0();
	// ��������� ��������� �������� ����, ��� ���� ��� �������� 
	// ��� ������� 50�� ����� ������ 
	// 180�/1023 = 0,01/1023 = 0.00000977 ��� 
	// ��� ������ ������� ������� ���� ��� �� 0� �� 180�
	// ���������� ������� ���� A (��� ������ ��� ������������� �� ���� A)
	// ��������� ���������� �� ������� ��������� OCR1A
	InitPhase(PHASE_TYPE_A);

}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// ���������� ���������� �� �������� ��������� OCR1C
// ��� ������ ����������� �������� ���������� �������� ���� (�������� ���� ���)


ISR(SIG_OUTPUT_COMPARE1A)
{	

}

// -------------------------------------------------------------------------------

void OnCompare1C()
{	
	


}

// -------------------------------------------------------------------------------
// ������������� �������� ����

void InitPhase(byte PhaseType)
{
	if (PhaseType >= PHASE_TYPE_COUNT)
		return;

	// ������������� ������� ���� ���� �� ������� (�� 0� �� 180�)
	Phase[PhaseType].DegreeCounter 	= 0;

	// ��������� �������������
	switch(PhaseType)
	{
		case PHASE_TYPE_A:
		case PHASE_TYPE_C: Phase[PhaseType].Polarity = POLARITY_NEGATIVE; break;
		case PHASE_TYPE_B: Phase[PhaseType].Polarity = POLARITY_POSITIVE; break;
	}
}

// -------------------------------------------------------------------------------
// ������ ������� ���� ��� �� 0� �� 180� ��� ���� (���������� - � +)

void SetPhasesDegree()
{
	int pt;

	// ���� �������� �������� �������� ���� � �������� 60�
	// �������� �������� ���� B
	if (Phase[PHASE_TYPE_A].DegreeCounter == 60)
		InitPhase(PHASE_TYPE_B);
		
	// ���� �������� �������� �������� ���� � �������� 120�
	// �������� �������� ���� C
	if (Phase[PHASE_TYPE_A].DegreeCounter == 120)
		InitPhase(PHASE_TYPE_C);
			
	// ������� ���� �,B � �
	for(pt = 0; pt < PHASE_TYPE_COUNT; pt++)
		UpdatePhaseState(pt); 

	// ���� ��� ���� ������ ��� ���� �� 180�
	// ����� ����������� ��������� �������� ����
	// � ����� ���������� �������������� ������� ��� ��������
	//if (EnableStopGenerator())
		//InterruptOCR1(OCR1_TYPE_C, INT_OCR1_DISABLE);

}

// -------------------------------------------------------------------------------

void UpdatePhaseState(byte PhaseType)
{

	// ���� �������� ��� ���������� �� 360� - ������ �� �������
	if (Phase[PhaseType].DegreeCounter == 360)
		return;
	
	

	//	if (	Phase[PHASE_TYPE_A].DegreeCounter == 1000 && 	
		//			Phase[PHASE_TYPE_A].Polarity == POLARITY_POSITIVE)
			//SetOutputState(OUTPUT_D19, OUTPUT_ON);
		

		if (Phase[PHASE_TYPE_A].DegreeCounter == 181)
			SetOutputState(OUTPUT_D19, OUTPUT_ON);
		

	// ���� �������� ������� � �������
	//if (Phase[PhaseType].DegreeCounter == ValueADC[0])
		//SetControlOutput(PhaseType, OUTPUT_GROUPE_A, OUTPUT_ON); // �� ����� ��� 1

	// ���� �������� ������� � �������
	//if (Phase[PhaseType].DegreeCounter == ValueADC[1])
		//SetControlOutput(PhaseType, OUTPUT_GROUPE_B, OUTPUT_ON); // �� ����� ��� 1

	// ���� ������� �������� ����� �� ������� �.� �� 180�
	if (Phase[PhaseType].DegreeCounter == 180)
	{
		// ������ ����������
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
	else // ���� ������� �������� �� ����� �� ������� �.� �� 180�
		Phase[PhaseType].DegreeCounter++; // �������� � ����������� � 180�
}

// -------------------------------------------------------------------------------
// ���������� ���������� ���������� �������� ���� (�������� ���� ���)

byte EnableStopGenerator()
{
	// ���� ��� ��� ���� ������ ������ ������ - 360�, ��
	// ���� ���������� �� �������� 
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

			// ������� � ������� ��������� ���������� �������
			// ����� ��������� �� ������� ��� 1, (������ ����������� ������)
			// � ����� �����, ���� �� ������� ���������� �� ������� ���������
			// ����� ���������� �� ���� �� ������ ��� 0, (�������� ����������� ������)
			LoadOCR1(OCR1Type, TiristorTickCount); 
			InterruptOCR1(OCR1Type, INT_OCR1_ENABLE); break;
	
      break;

		case OUTPUT_OFF: 	
      TiristorPhase = PHASE_TYPE_UNDEFINED; 
			
			// ������ ���������� �� �������� ��������� ������
	    // ��������� ���������� ��������� � ���������
			// ����� ��������� �� ������ ��� 0
			// �������� ��� ����� ������������ �������
			switch(GroupeType)
			{
				case OUTPUT_GROUPE_A: InterruptOCR1(OCR1_TYPE_A, INT_OCR1_DISABLE); break;
				case OUTPUT_GROUPE_B: InterruptOCR1(OCR1_TYPE_B, INT_OCR1_DISABLE); break;
			}

      break;
	}

	// �������������� �� ������� (� ��������� �� ����������) ���� ��� 0 ���� ��� 1, 
	// ����� ��������� ��� ���������� �� ������ ���������� ���� 
	// ��� 1 ����� ���������� � ��� 0
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
// ���������� ���������� �� �������� ��������� OCR1A
// ��������� ������������� �������� ��� ������� ������������ ��������� ������ �
// ��� ������ ���������� �� ������ ������ 
// � ���������� �� ���� ������� ��� 0

ISR(SIG_OUTPUT_COMPARE1A)
{
	if (TiristorPhase >= PHASE_TYPE_COUNT)
		return;

	// ������������� �� ������ ��� 0
	SetControlOutput(TiristorPhase, OUTPUT_GROUPE_A, OUTPUT_OFF);
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// ���������� ���������� �� �������� ��������� OCR1B
// ��������� ������������� �������� ��� ������� ������������ ��������� ������ B
// ��� ������ ���������� �� ������ ������ 
// � ���������� �� ���� ������� ��� 0

ISR(SIG_OUTPUT_COMPARE1B)
{
	if (TiristorPhase >= PHASE_TYPE_COUNT)
		return;

	// ������������� �� ������ ��� 0
	SetControlOutput(TiristorPhase, OUTPUT_GROUPE_B, OUTPUT_OFF);
}

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
// ������� �-� ������ ���������� ���������

int main(void)
{	
	// --------- ��������� ��� ������� �� ������������������ ������ --------- 

	LoadSettings();

	 // ���������� ������ � �������� �� 360� � ����������� �� ������� �������
	PeriodPhase360 = 1/50.0;//1/Settings[SETTING_F_POWER].PhysicalVal; 

	// ���������� ����� � ���������� ��������� ��� ������� ��������� 
	double TimeoutTiristor = Settings[SETTING_T_TIRISTOR].PhysicalVal;
	TiristorTickCount = SecondToTick(	TC1, DIV_TC1_CLK, 
																		TimeoutTiristor / 1000000.0 );

	// --------- ������������� ������� �������� 0 --------- 

	TC0_TickCount = SecondToTick(	TC0, DIV_TC0_CLK1024,  PeriodPhase360 / 4 );

	TC0_TickCount = 0x100 - TC0_TickCount;

	// --------- ������������� ������� �������� 1 --------- 

	InitTimer1(DIV_TC1_CLK);
	
	// ��������� ���������� ��������� ��� ���������� �������� ����
	// ��� ������� 50�� ����� ������, ��� ��� �������� 
	// 180�/1023 = 0,01/1023 = 0.00000977 ��� 
	TC1_TickCount = SecondToTick( TC1, DIV_TC1_CLK, PeriodPhase360 / 2 / 360 );

	// --------- ������������� ������ ��� ���������� ����������� --------- 
	
	int pt, OutputA = 0, OutputB = 0;
	for(pt = 0; pt < PHASE_TYPE_COUNT; pt++)
	{
		Phase[pt].Output1[OUTPUT_GROUPE_A] 	= OutputGroupeA[OutputA++];
		Phase[pt].Output2[OUTPUT_GROUPE_A]	= OutputGroupeA[OutputA++];

		Phase[pt].Output1[OUTPUT_GROUPE_B] 	= OutputGroupeB[OutputB++];
		Phase[pt].Output2[OUTPUT_GROUPE_B]	= OutputGroupeB[OutputB++];
	}

	// --------- ����������� ����� A,C � G ��� ������ --------- 
	
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRG = 0xFF;

	// --------- ������������� ��� --------- 
	// ��������� ��� ������� ���������� � 0-�� �����
	// ���� REFS � ADLAR ��������
	// ���� ADPS �������� ADCSRA = 0b00000111 ������, ���
	// �������� ������� ��� ������ 16��� �������� �� 128
	// ��� ��� �������� ��� ���������� ������ ���
	//	������ �������� � ��������� �� 50 �� 200���
	// 16000000/128 = 125000 �� = 125���
	ADMUX 		= ADCInputs[ADCInput];
	ADCSRA 		= 0b11001111;

	// --------- ���������� ���������� INT0 (�������������) --------- 
	// �� ����������� ������ ��� ������ ���
	// ����� ICS0 �������� EICRA � ��������� 11
	// ��������� INT0 ����� ���������� ��� ��� ����������, ������� 
	// ��������� ���������� INT0 ����� ������ ���� 
	// ���������� ���������� � �������� EIMSK
	// ���������� ��� 1 �� ���� ���������� INT0 �������� EIFR
	// ����� �������� ���� ����������

	EIMSK = 0x0; 		// ���������� ���������� INT0

	EICRA = 0x03; 	// ����� ICS0 �������� EICRA � ��������� 11
	EIFR  = 0x01; 	// ���������� ���� ����������

	EIMSK = 0x01; 	// ��������� ���������� INT0

	// --------- ����� ���������� ���� ���������� --------- 
	sei();

	// USART initialization
	// Communication Parameters: 8 Data, No Parity, 1 Stop  8N1
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	UCSR0A=0x00;
	UCSR0B=0x98;
	UCSR0C=0x86;

	// USART Baud rate: 115200  ��� ������ 11,059 ���
	UBRR0H=0x00;
	UBRR0L=0x67; 

	// --------- ����������� ���� --------- 
	while(1);
		
	return 0;
}

// -------------------------------------------------------------------------------
