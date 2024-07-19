#include "UnitTest.h"


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