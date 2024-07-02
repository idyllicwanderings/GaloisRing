#define JOIN(a, b)                      JOIN0(a, b)

#define performTestSponge               JOIN(prefix, _performTestSponge)
#define selfTestSponge                  JOIN(prefix, _selfTestSponge)

#define inputKDegree 32


void performTestSponge(unsigned int rate)
{
    unsigned char input[inputByteSize];
    unsigned int inputBitLength, offset, size;
    int result;

    /* Acumulated test vector */
    SpongeInstance spongeChecksum;
    SpongeInitialize(&spongeChecksum, SnP_width, 0);

    for(inputBitLength = 0; inputBitLength <= (inputByteSize*8); inputBitLength += (inputBitLength < SnP_width) ? 1 : 8) {
        unsigned int i;
        unsigned int inputByteLengthCeiling = (inputBitLength + 7) / 8;

        for(i=0; i<inputByteLengthCeiling; i++)
            input[i] = inputBitLength - i;
        if ((inputBitLength % 8) != 0)
            input[inputByteLengthCeiling-1] &= (1 << (inputBitLength % 8)) - 1;

        if ((flavor == flavor_IUF_AllAtOnce) || (flavor == flavor_IUF_Pieces)) {
            SpongeInstance sponge;
            result = SpongeInitialize(&sponge, rate, capacity);
            assert(result == 0);

            if (flavor == flavor_IUF_Pieces) {
                for(offset = 0; offset < inputBitLength/8; offset += size) {
                    unsigned int p = (inputBitLength/8)%11;
                    /* vary sizes */
                    if (p < 1) size = 1; /* byte per byte */
                    else if (p < 5) size = (rand() % 20); /* small random */
                    else if (p < 9) size = (rand() % 200); /* big random */
                    else size = ((rand() % (inputBitLength/8 - offset)) + 1); /* converging random */

                    if (size > (inputBitLength/8 - offset))
                        size = inputBitLength/8 - offset;

                    result = SpongeAbsorb(&sponge, input + offset, size);
                    assert(result == 0);
                }
            }
            else {
                result = SpongeAbsorb(&sponge, input, inputBitLength/8);
                assert(result == 0);
            }
            if ((inputBitLength % 8) != 0) {
                result = SpongeAbsorbLastFewBits(&sponge, input[inputByteLengthCeiling-1] | (1 << (inputBitLength % 8)));
                assert(result == 0);
            }

            if (flavor == flavor_IUF_Pieces) {
                unsigned char filler = 0xAA + inputBitLength;
                memset(output, filler, sizeof(output));
                for(offset = 0; offset < outputByteSize; offset += size) {
                    unsigned int p = (inputBitLength/8)%11;
                    /* vary sizes */
                    if (p < 1) size = 1; /* byte per byte */
                    else if (p < 5) size = (rand() % 20); /* small random */
                    else if (p < 9) size = (rand() % 200); /* big random */
                    else size = ((rand() % (outputByteSize - offset)) + 1); /* converging random */

                    if (size > (outputByteSize - offset))
                        size = outputByteSize - offset;

                    result = SpongeSqueeze(&sponge, output + offset, size);
                    assert(result == 0);
                    for(i = offset + size; i<sizeof(output); i++)
                        if (output[i] != filler) {
                            #ifndef UT_EMBEDDED
                            printf("Out of range data written!\n");
                            #endif
                            abort();
                        }
                }
            }
            else {
                result = SpongeSqueeze(&sponge, output, outputByteSize);
                assert(result == 0);
            }
        }
        else if (flavor == flavor_OneCall) {
            unsigned char suffix = ((inputBitLength % 8) != 0) ?
                input[inputByteLengthCeiling-1] | (1 << (inputBitLength % 8)) : 0x01;
            result = Sponge(rate, capacity, input, inputBitLength/8, suffix, output, outputByteSize);
            assert(result == 0);
        }

        SpongeAbsorb(&spongeChecksum, output, outputByteSize);
    }
    SpongeSqueeze(&spongeChecksum, checksum, checksumByteSize);

}