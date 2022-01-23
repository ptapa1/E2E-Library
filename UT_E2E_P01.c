/** ==================================================================================================================*\
  @file UT_E2E_P1.c

  @brief Testy jednostkowe do E2E_P1
\*====================================================================================================================*/

#include "Std_Types.h"

#include "acutest.h"
#include "fff.h"

#include "E2E_P1.c"


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
    uint8 counter = 0;
    uint8 crc;
    Crc_CalculateCRC8_fake.return_val = 0x5;
    crc = CalculateCrc(&Config, counter, data);

    TEST_CHECK(Crc_CalculateCRC8_fake.call_count == 3);
    TEST_CHECK(crc == (5 ^ 0xFF));
}


TEST_LIST = {
    { "Test of CalculateCrc", Test_Of_CalculateCrc },   /* Format to { "nazwa testu", nazwa_funkcji } */
    { NULL, NULL }                                      /* To musi być na końcu */
};

