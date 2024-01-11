

// ------------------------------------------------------------------------------

#define DIAG_DISCRETE_PORT_COUNT 				7

#define DIAG_DISCRETE_PORT_TYPE_IO 			0
#define DIAG_DISCRETE_PORT_TYPE_DATA		1
#define DIAG_DISCRETE_PORT_TYPE_PIN 		2

#define DIAG_DISCRETE_PORT_TYPE_COUNT 	3

#define DIAG_DISCRETE_INPUT_COUNT 			8


#define DIAG_DISCRETE_PORTS_ARRAY_LEN  	DIAG_DISCRETE_PORT_COUNT



// ------------------------------------------------------------------------------

#define DIAG_ANALOG_INPUT_COUNT 				4

#define DIAG_ANALOG_INPUTS_ARRAY_LEN  	DIAG_ANALOG_INPUT_COUNT * sizeof(WORD)

// ------------------------------------------------------------------------------


#define DIAG_ARRAY_LEN  								DIAG_DISCRETE_PORTS_ARRAY_LEN + DIAG_ANALOG_INPUTS_ARRAY_LEN

// ------------------------------------------------------------------------------

#define ONE_SECOND_TICK_COUNT						60 			// ������� ����� 61 ����, ��� ����� �������
														 										// �� 1 ���� ������ �� ���������� �-� 

#define HALF_SECOND_TICK_COUNT 					ONE_SECOND_TICK_COUNT / 2 // ��� �������


#define INPUT_ADC_COUNT									4 	// ���������� ������������� 
																						// ���������� ������ 

// ------------------------------------------------------------------------------
void StartDiagPorts();

void InitTimer0();
void InitTimer2();
void InitADC();

// ------------------------------------------------------------------------------

void GetDiscreteInputsState();
void SetDiscreteInputState(BYTE Port, BYTE Input, BOOL State);

// ------------------------------------------------------------------------------

void SendPortIO();

// ------------------------------------------------------------------------------


