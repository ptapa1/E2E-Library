/** ==================================================================================================================*\
  @file UT_E2E_P1.c

  @brief Testy jednostkowe do E2E_P1
\*====================================================================================================================*/

#include "Std_Types.h"

#include "acutest.h"
#include "fff.h"

#include "E2E_P01.c"


DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC4(uint8, Crc_CalculateCRC8,const uint8*, uint32, uint8, boolean);

/**
  @brief Test obliczania crc
*/
void Test_Of_CalculateCrc(void)
{
    E2E_P01ConfigType Config;
    Config = (E2E_P01ConfigType){
        .CounterOffset = 8,
        .CRCOffset = 0,
        .DataID = 0xA4,
        .DataIDMode = E2E_P01_DATAID_BOTH,
        .DataLength = 64,
        .MaxDeltaCounterInit = 1
    };
    uint8 data[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    uint8 counter = 2;
    uint8 call_count = 0;
    uint8 crc;
    Crc_CalculateCRC8_fake.return_val = 0x5;
    crc = CalculateCrc(&Config, counter, data);
    
    TEST_CHECK((Crc_CalculateCRC8_fake.call_count - call_count) == 3 );
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[0] == 0xFF);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[2] == 0x5);
    TEST_CHECK(crc == (5 ^ 0xFF));
    call_count = Crc_CalculateCRC8_fake.call_count;

    Config.DataIDMode = E2E_P01_DATAID_LOW;
    crc = CalculateCrc(&Config, counter, data);
    TEST_CHECK((Crc_CalculateCRC8_fake.call_count - call_count) == 2);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[0] == 0xFF);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(crc == (5 ^ 0xFF));
    call_count = Crc_CalculateCRC8_fake.call_count;

    Config.DataIDMode = E2E_P01_DATAID_ALT;
    crc = CalculateCrc(&Config, counter, data);
    TEST_CHECK((Crc_CalculateCRC8_fake.call_count - call_count) == 2);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[0] == 0xFF);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(crc == (5 ^ 0xFF));
    call_count = Crc_CalculateCRC8_fake.call_count;

    Config.DataIDMode = E2E_P01_DATAID_NIBBLE;
    crc = CalculateCrc(&Config, counter, data);
    TEST_CHECK((Crc_CalculateCRC8_fake.call_count - call_count) == 3);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[0] == 0xFF);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(crc == (5 ^ 0xFF));
    call_count = Crc_CalculateCRC8_fake.call_count;

    Config.DataIDMode = E2E_P01_DATAID_ALT;
    counter = 3;
    crc = CalculateCrc(&Config, counter, data);
    TEST_CHECK((Crc_CalculateCRC8_fake.call_count - call_count) == 2);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[0] == 0xFF);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(crc == (5 ^ 0xFF));
    call_count = Crc_CalculateCRC8_fake.call_count;

    Config.DataIDMode = E2E_P01_DATAID_ALT;
    Config.CRCOffset = 100;
    crc = CalculateCrc(&Config, counter, data);
    TEST_CHECK((Crc_CalculateCRC8_fake.call_count - call_count) == 2);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[0] == 0xFF);
    TEST_CHECK(Crc_CalculateCRC8_fake.arg2_history[1] == 0x5);
    TEST_CHECK(crc == (5 ^ 0xFF));
    call_count = Crc_CalculateCRC8_fake.call_count;
}

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

    retv = E2E_P01Protect(NULL, &State, &Data);
    TEST_CHECK(retv == E2E_E_INPUTERR_NULL);

    Config.CounterOffset = 12;
    retv = E2E_P01Protect(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);

    Config.CounterOffset = 8;
    Config.DataIDMode = E2E_P01_DATAID_NIBBLE;
    retv = E2E_P01Protect(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);

    Config.DataIDNibbleOffset = 16;
    retv = E2E_P01Protect(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);
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

    Config.DataLength = 65;
    retv = CheckConfig(&Config);
    TEST_CHECK(retv == E2E_E_INPUTERR_WRONG);

    Config.DataLength = 64;
    Config.CRCOffset = 64;
    retv = CheckConfig(&Config);
    TEST_CHECK(retv == E2E_E_INPUTERR_WRONG);
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

    retv = E2E_P01Check(NULL, &State, &Data);
    TEST_CHECK(retv == E2E_E_INPUTERR_NULL);

    State.NewDataAvailable = 0x0;
    State.MaxDeltaCounter = 0xA;
    retv = E2E_P01Check(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);

    State.NewDataAvailable = 0x1;
    Config.CounterOffset = 12;
    State.WaitForFirstData = 0x1;
    retv = E2E_P01Check(&Config, &State, &Data);
    TEST_CHECK(retv == E2E_E_OK);
}


TEST_LIST = {
    { "Test of CalculateCrc", Test_Of_CalculateCrc },
    { "Test of E2E_P01Protect", Test_of_E2E_P01Protect },
    { "Test of CheckConfig", Test_of_CheckConfig },
    { "Test of CalculateDeltaCounter", Test_of_CalculateDeltaCounter },
    { "Test of E2E_P01Check", Test_of_E2E_P01Check },/* Format to { "nazwa testu", nazwa_funkcji } */
    { NULL, NULL }                                      /* To musi być na końcu */
};

