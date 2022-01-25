#include "Std_Types.h"
#include "E2E_P01.h"
#include "E2E.h"

#define MAX_P01_DATA_LENGTH_IN_BITS (240)
#define MAX_P01_COUNTER_VALUE (14)

static inline Std_ReturnType CheckConfig(E2E_P01ConfigType* Config) {

    if (Config == NULL) {
        return E2E_E_INPUTERR_NULL; 
    }

    if ((Config->DataLength > MAX_P01_DATA_LENGTH_IN_BITS) || (Config->DataLength % 8 != 0)  ||
        (Config->CounterOffset % 4 != 0) || Config->CRCOffset % 8 != 0) {
        return E2E_E_INPUTERR_WRONG; 
    }

    if ((Config->CRCOffset + 8 > Config->DataLength) || (Config->CounterOffset + 4 > Config->DataLength) ||
        (Config->CRCOffset/8 == Config->CounterOffset/8)) {
        return E2E_E_INPUTERR_WRONG; 
    }

    return E_OK;
	
}

static uint8 CalculateCrc(E2E_P01ConfigType* Config, uint8 Counter, uint8* Data)
{
    uint8 crc = 0x00;
    uint8 lowerByteId = (uint8)(Config->DataID);
    uint8 upperByteId = (uint8)(Config->DataID>>8);

    /* Calculate CRC on the Data ID */
    switch (Config->DataIDMode) 
    {
        case E2E_P01_DATAID_BOTH:
            crc = Crc_CalculateCRC8(&lowerByteId, 1, 0xFF, FALSE);
            crc = Crc_CalculateCRC8(&upperByteId, 1, crc, FALSE);
            break;
        case E2E_P01_DATAID_LOW:
            crc = Crc_CalculateCRC8(&lowerByteId, 1, 0xFF, FALSE);
            break;
        case E2E_P01_DATAID_ALT:
            if (Counter % 2 == 0)
            {
                crc = Crc_CalculateCRC8(&lowerByteId, 1, 0xFF, FALSE);
            }
            else
            {
                crc = Crc_CalculateCRC8(&upperByteId, 1, 0xFF, FALSE);
            }
            break;
        case E2E_P01_DATAID_NIBBLE:
            crc = Crc_CalculateCRC8(&lowerByteId, 1, 0xFF, FALSE);
            crc = Crc_CalculateCRC8(&upperByteId, 1, crc, FALSE);
            break;
        default:
            break;
    }

    /* Calculate CRC on the data */
    if (Config->CRCOffset >= 8) {
        crc = Crc_CalculateCRC8 (Data, (Config->CRCOffset / 8), crc, FALSE);
    }

    if (Config->CRCOffset / 8 < (Config->DataLength / 8) - 1) {
        crc = Crc_CalculateCRC8 (&Data[Config->CRCOffset/8 + 1],
                                (Config->DataLength / 8 - Config->CRCOffset / 8 - 1),
                                crc, FALSE);
    }

    return crc ^ 0xFF;

}

Std_ReturnType E2E_P01Protect(E2E_P01ConfigType* Config, E2E_P01SenderStateType* State, uint8* Data) {

    Std_ReturnType returnValue = CheckConfig(Config);

    if (E2E_E_OK != returnValue) {
        return returnValue;
    }

    if ((State == NULL) || (Data == NULL)) {
        return E2E_E_INPUTERR_NULL;
    }

    if (Config->CounterOffset % 8 == 0) {
        *(Data+(Config->CounterOffset/8)) = (*(Data+(Config->CounterOffset/8)) & 0xF0) | (State->Counter & 0x0F);
    }
    else {
        *(Data+(Config->CounterOffset/8)) = (*(Data+(Config->CounterOffset/8)) & 0x0F) | ((State->Counter<<4) & 0xF0);
    }

    if(Config->DataIDMode == E2E_P01_DATAID_NIBBLE){
        if(Config->DataIDNibbleOffset % 8 == 0){
            *(Data+(Config->DataIDNibbleOffset/8)) = (*(Data+(Config->DataIDNibbleOffset/8)) & 0xF0) | ((Config->DataID>>8) & 0x0F);
        }
        else{
            *(Data+(Config->DataIDNibbleOffset/8)) = (*(Data+(Config->DataIDNibbleOffset/8)) & 0x0F) | ((Config->DataID>>4) & 0xF0);
        }
    }
   
    *(Data+(Config->CRCOffset/8)) = CalculateCrc(Config, State->Counter, Data);

    State->Counter = E2E_UpdateCounter(State->Counter);

    return E2E_E_OK;
}

uint8 E2E_UpdateCounter(uint8 Counter) {
    return (Counter+1) % 15; // profile 1 value
}

static inline uint8 CalculateDeltaCounter(uint8 receivedCounter, uint8 lastValidCounter)
{
    if (receivedCounter >= lastValidCounter) {
        return receivedCounter - lastValidCounter;
    }
    else {
        return MAX_P01_COUNTER_VALUE + 1 + receivedCounter - lastValidCounter;
    }
}

Std_ReturnType E2E_P01Check(E2E_P01ConfigType* Config, E2E_P01ReceiverStateType* State, uint8* Data) {

    uint8 receivedCounter = 0;
    uint8 receivedCrc = 0;
    uint8 calculatedCrc = 0;
    uint8 delta = 0;
    Std_ReturnType returnValue = CheckConfig(Config);

    if (E2E_E_OK != returnValue) {
        return returnValue;
    }

    if ((State == NULL) || (Data == NULL)) {
        return E2E_E_INPUTERR_NULL;
    }

    if (State->MaxDeltaCounter < MAX_P01_COUNTER_VALUE) {
        State->MaxDeltaCounter++;
    }

    if (State->NewDataAvailable == FALSE) {
        State->Status = E2E_P01STATUS_NONEWDATA;
        return E2E_E_OK;  
    }

    if (Config->CounterOffset % 8 == 0) {
        receivedCounter = *(Data+(Config->CounterOffset/8)) & 0x0F;
    }
    else {
        receivedCounter = (*(Data+(Config->CounterOffset/8)) >> 4) & 0x0F;
    }

    receivedCrc = *(Data+(Config->CRCOffset/8));
    calculatedCrc = CalculateCrc(Config, receivedCounter, Data);

    if (receivedCrc != calculatedCrc) {
        State->Status = E2E_P01STATUS_WRONGCRC;
        return E2E_E_OK;
    }

    if (State->WaitForFirstData == TRUE) {
        State->WaitForFirstData = FALSE;
        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
        State->LastValidCounter = receivedCounter;
        State->Status= E2E_P01STATUS_INITAL;
        return E2E_E_OK;
    }

    delta = CalculateDeltaCounter(receivedCounter, State->LastValidCounter);

    if (delta == 1) {
        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
        State->LastValidCounter = receivedCounter;
        State->LostData = 0;
        State->Status= E2E_P01STATUS_OK;
    }
    else if (delta == 0) {
        State->Status= E2E_P01STATUS_REPEATED;
    }
    else if (delta <= State->MaxDeltaCounter) {
        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
        State->LastValidCounter = receivedCounter;
        State->LostData = delta - 1;
        State->Status= E2E_P01STATUS_OKSOMELOST;
    }
    else {
        State->Status= E2E_P01STATUS_WRONGSEQUENCE;
    }

    return E2E_E_OK;
}

