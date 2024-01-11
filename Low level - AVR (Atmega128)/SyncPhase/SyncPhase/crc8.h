#ifndef _CRC8_H_
#define _CRC8_H_

#include "SyncPhase.h"

extern const byte CRC8_Table[256];

byte CalcCRC8(byte* ptr, int len);

#endif /* _CRC8_H_ */
