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
//void Test_of_protect(void){
//
//}

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

TEST_LIST = {
    { "Test of CheckConfig", Test_of_CheckConfig },   /* Format to { "nazwa testu", nazwa_funkcji } */
    { NULL, NULL }                                      /* To musi byæ na koñcu */
};
