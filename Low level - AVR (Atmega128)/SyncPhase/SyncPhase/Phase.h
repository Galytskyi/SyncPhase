#ifndef _PHASE_H_
#define _PHASE_H_

#include "SyncPhase.h"
#include "Signal.h"

// ��� ����

#define 			PHASE_TYPE_UNDEFINED	-1
#define 			PHASE_TYPE_A 					0
#define 			PHASE_TYPE_B 					1
#define 			PHASE_TYPE_C 					2
															
#define 			PHASE_TYPE_COUNT  		3

// ���������� ����

#define 			POLARITY_UNDEFINED 	 	-1
#define 			POLARITY_NEGATIVE 	 	0
#define 			POLARITY_POSITIVE 	 	1

#define 			POLARITY_COUNT  			2

// ��������� ����

typedef struct _PHASE
{
	
	uint		DegreeCounter; 			// ������� ��������
	byte		Polarity; 					// ���������

	byte 		Output1[OUTPUT_GROUPE_COUNT];
	byte 		Output2[OUTPUT_GROUPE_COUNT];

} PHASE;


#endif /* _PHASE_H_ */
