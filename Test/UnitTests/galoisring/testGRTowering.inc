

# define GRT_HAS_5_TOWERS
#define k
#define d1
#define d2
#define d3
#define d4
#define d5


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