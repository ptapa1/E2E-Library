/** ==================================================================================================================*\
  @file UT_E2E_P1.c

  @brief Testy jednostkowe do E2E_P1
\*====================================================================================================================*/

#include "Std_Types.h"

#include "acutest.h"
#include "fff.h"

#include "E2E_P01.c"

DEFINE_FFF_GLOBALS;

/**
  @brief Test obliczania crc
*/
void Test_of_E2E_P01Protect(void){
    Std_ReturnType retv;
    E2E_P01ConfigType Config;
    E2E_P01SenderStateType State;
    uint8 Data;
    Config = (E2E_P01ConfigType){
        .CounterOffset = 8,
        .CRCOffset = 0,
        .DataID = 0xA4,
        .DataIDNibbleOffset = 12,
        .DataIDMode = E2E_P01_DATAID_BOTH,
        .DataLength = 64,
        .MaxDeltaCounterInit = 1
    };
    State = (E2E_P01SenderStateType){
        .Counter = 0
    };
    Data = 0;
    retv = E2E_P01Protect(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);

    retv = E2E_P01Protect(&Config, &State, NULL);
    TEST_CHECK(retv == E2E_E_INPUTERR_NULL);
}

void Test_of_CheckConfig(void){
    Std_ReturnType retv;
    E2E_P01ConfigType Config;
    Config = (E2E_P01ConfigType){
        .CounterOffset = 8,
        .CRCOffset = 0,
        .DataID = 0xA4,
        .DataIDNibbleOffset = 12,
        .DataIDMode = E2E_P01_DATAID_BOTH,
        .DataLength = 64,
        .MaxDeltaCounterInit = 1
    };
    retv = CheckConfig(&Config);
    TEST_CHECK(retv == E2E_E_OK);

    retv = CheckConfig(NULL);
    TEST_CHECK(retv == E2E_E_INPUTERR_NULL);
}

void Test_of_CalculateDeltaCounter(void){
    uint8 receivedCounter  = 0x5;
    uint8 lastValidCounter = 0x4;
    uint8 retv;

    retv = CalculateDeltaCounter(receivedCounter, lastValidCounter);
    TEST_CHECK(retv == receivedCounter - lastValidCounter);

    receivedCounter  = 0x4;
    lastValidCounter = 0x5;

    retv = CalculateDeltaCounter(receivedCounter, lastValidCounter);
    TEST_CHECK(retv == MAX_P01_COUNTER_VALUE + 1 + receivedCounter - lastValidCounter);
    
}

void Test_of_E2E_P01Check(void){
    Std_ReturnType retv;
    E2E_P01ConfigType Config;
    E2E_P01ReceiverStateType State;
    uint8 Data;
    Config = (E2E_P01ConfigType){
        .CounterOffset = 8,
        .CRCOffset = 0,
        .DataID = 0xA4,
        .DataIDNibbleOffset = 12,
        .DataIDMode = E2E_P01_DATAID_BOTH,
        .DataLength = 64,
        .MaxDeltaCounterInit = 1
    };
    State = (E2E_P01ReceiverStateType){
        .LastValidCounter = 0x5,
        .MaxDeltaCounter  = 0xE,
        .WaitForFirstData = 0x1,
        .NewDataAvailable = 0x1,
        .LostData         = 0x0,
        .Status           = E2E_P01STATUS_OK
    };
    Data = 0;


    retv = E2E_P01Check(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);

    retv = E2E_P01Check(&Config, &State, NULL);
    TEST_CHECK(retv == E2E_E_INPUTERR_NULL);
}

TEST_LIST = {
    { "Test of E2E_P01Protect", Test_of_E2E_P01Protect },
    { "Test of CheckConfig", Test_of_CheckConfig },
    { "Test of CalculateDeltaCounter", Test_of_CalculateDeltaCounter },
    { "Test of E2E_P01Check", Test_of_E2E_P01Check },/* Format to { "nazwa testu", nazwa_funkcji } */
    { NULL, NULL }                                      /* To musi byæ na koñcu */
};
