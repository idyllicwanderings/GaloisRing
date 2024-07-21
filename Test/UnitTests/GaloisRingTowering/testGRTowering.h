#ifndef _TEST_GRTOWERING_H_
#define _TEST_GRTOWERING_H_

#include "../UnitTest.h"
#include "../lib/gring.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>

#define k 2
#define d1 5
#define d2 5
#define d3 5
#define d4 5
#define d5 5
#define GRT_HAS_5_TOWERS


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
