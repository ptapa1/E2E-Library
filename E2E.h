#ifndef E2E_H_
#define E2E_H_

#include "Std_Types.h"

uint8 Crc_CalculateCRC8(const uint8* Crc_DataPtr, uint32 Crc_Length, uint8 Crc_StartValue8, boolean Crc_IsFirstCall);

#define E2E_E_INPUTERR_NULL 0x13
#define E2E_E_INPUTERR_WRONG 0x17
#define E2E_E_INTERR 0x19
#define E2E_E_OK 0x00
#define E2E_E_INVALID 0xFF

#endif
