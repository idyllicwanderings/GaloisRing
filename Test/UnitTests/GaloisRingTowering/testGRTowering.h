#ifndef _TEST_GRTOWERING_H_
#define _TEST_GRTOWERING_H_

#include "../UnitTest.h"
#include "../lib/gring.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>

#define k 7
#define d1 2
#define d2 3
#define d3 2
// #define d4 2
// #define d5 2
// #define GRT_HAS_1_TOWER
// #define GRT_HAS_2_TOWERS
 #define GRT_HAS_3_TOWERS
#define ADD_TV_FILE "test/TestVectors/GRToweringAddition.txt"
#define SUB_TV_FILE "test/TestVectors/GRToweringSubtraction.txt"
#define MUL_TV_FILE "test/TestVectors/GRToweringMultiplication.txt"
#define INV_TV_FILE "test/TestVectors/GRToweringInverse.txt"



#ifdef UT_OUTPUT
void writeTestSpongePRG(FILE *f, unsigned int rhoInBytes)
{

    // performTestSpongePRG(rhoInBytes, flavor_IUF_AllAtOnce, checksum);
    // fprintf(f, "    selfTestSpongePRG(%d, %d, flavor, \"", rhoInBytes, SnP_width);
    // for(offset=0; offset<checksumByteSize; offset++)
    //     fprintf(f, "\\x%02x", checksum[offset]);
    // fprintf(f, "\"); /* Keccak[r=%d, c=%d] */\n", rhoInBytes*8+2, SnP_width-(rhoInBytes*8+2) );
}
#endif

void testDuplex(void);

void testGR(void);

void selfTestAddition(std::string in_a, std::string in_b, std::string expected_c);

void selfTestSubtraction(std::string in_a, std::string in_b, std::string expected_c);

void selfTestMultiplication(std::string in_a, std::string in_b, std::string expected_c);

void selfTestInverse(std::string in_a, std::string in_b);


#endif
