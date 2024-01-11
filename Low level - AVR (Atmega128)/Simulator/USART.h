
// ----------------------------------------------------------

#define USART_BAUD 9600ul
#define USART_UBBR_VALUE ((F_CPU/(USART_BAUD<<4))-1)


// ----------------------------------------------------------

void 		InitUSART();							// Инициализация 
BYTE 		ReceiveByte();						// Принять байт
void 		SendByte(BYTE Data);			// Отправить байт

// ----------------------------------------------------------

#define MSG_TYPE_STR 										0
#define MSG_TYPE_DATA										1

#define MSG_TYPE_STR_USUAL							0
#define MSG_TYPE_STR_WARNING						1
#define MSG_TYPE_STR_ERROR							2

#define MSG_TYPE_DATA_PORT_IO						0
#define MSG_TYPE_DATA_PORT_STATE				1
#define MSG_TYPE_DATA_EEPROM						2

// ----------------------------------------------------------

#define USART_RECV_BUFFER_LEN  					0xFF

// ----------------------------------------------------------

#define REQUEST_GET_PORT_IO							0
#define REQUEST_SET_DISCRETE_STATE			1
#define REQUEST_GET_EEPROM_PARAM				2
#define REQUEST_SET_EEPROM_PARAM				3


#define REQUEST_LEN											8
#define REQUEST_DATA_LEN								4


struct MSG_REQUEST
{
	BYTE	Type;
	BYTE	DataLen;
	BYTE	CRC8;
	BYTE	Data[REQUEST_DATA_LEN];
	BYTE	End;
};


#define MSG_REQUEST_LEN								sizeof(MSG_REQUEST)

// ----------------------------------------------------------

// Передача одного байта 15,5 мс 
	
void 		SendStr(char* pStr, BYTE StrType);
void 		SendData(BYTE* pBuffer, BYTE BufferLength, BYTE DataType);


// ----------------------------------------------------------
