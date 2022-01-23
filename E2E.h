#ifndef E2E_H_
#define E2E_H_


#include "Std_Types.h"

uint8 Crc_CalculateCRC8(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8, boolean Crc_IsFirstCall);

#endif