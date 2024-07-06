#define JOIN(a, b)                      JOIN0(a, b)

#define performTestSponge               JOIN(prefix, _performTestSponge)
#define selfTestSponge                  JOIN(prefix, _selfTestSponge)

#define inputKDegree 32



STATUS_CODES
genShortMsgHash(unsigned int rate, unsigned int capacity, unsigned char delimitedSuffix, unsigned int hashbitlen, unsigned int squeezedOutputLength, const char *fileName, const char *description)
{
    int         msglen, msgbytelen;
    BitSequence Msg[256];
    BitSequence Squeezed[SqueezingOutputLength/8];
    Keccak_HashInstance   hash;
    FILE        *fp_in, *fp_out;

    if ((squeezedOutputLength > SqueezingOutputLength) || (hashbitlen > SqueezingOutputLength)) {
        printf("Requested output length too long.\n");
        return KAT_HASH_ERROR;
    }

    if ( (fp_in = fopen("ShortMsgKAT.txt", "r")) == NULL ) {
        printf("Couldn't open <ShortMsgKAT.txt> for read\n");
        return KAT_FILE_OPEN_ERROR;
    }

    if ( (fp_out = fopen(fileName, "w")) == NULL ) {
        printf("Couldn't open <%s> for write\n", fileName);
        return KAT_FILE_OPEN_ERROR;
    }
    fprintf(fp_out, "# %s\n", description);

    do {
        if ( FindMarker(fp_in, "Len = ") )
            fscanf(fp_in, "%d", &msglen);
        else {
            break;
        }
        msgbytelen = (msglen+7)/8;

        if ( !ReadHex(fp_in, Msg, msgbytelen, (char*)"Msg = ") ) {
            printf("ERROR: unable to read 'Msg' from <ShortMsgKAT.txt>\n");
            return KAT_DATA_ERROR;
        }
        fprintf(fp_out, "\nLen = %d\n", msglen);
        fprintBstr(fp_out, (char*)"Msg = ", Msg, msgbytelen);

        if (Keccak_HashInitialize(&hash, rate, capacity, hashbitlen, delimitedSuffix) != KECCAK_SUCCESS) {
            printf("Keccak[r=%d, c=%d] is not supported.\n", rate, capacity);
            return KAT_HASH_ERROR;
        }
        Keccak_HashUpdate(&hash, Msg, msglen);
        Keccak_HashFinal(&hash, Squeezed);
        if (hashbitlen > 0)
            fprintBstr(fp_out, (char*)"MD = ", Squeezed, hashbitlen/8);
        if (squeezedOutputLength > 0) {
            Keccak_HashSqueeze(&hash, Squeezed, squeezedOutputLength);
            fprintBstr(fp_out, (char*)"Squeezed = ", Squeezed, squeezedOutputLength/8);
        }
    } while ( 1 );
    printf("finished ShortMsgKAT for <%s>\n", fileName);

    fclose(fp_in);
    fclose(fp_out);

    return KAT_SUCCESS;
}

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