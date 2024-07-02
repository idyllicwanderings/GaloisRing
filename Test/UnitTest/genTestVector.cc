

int genZ2kInStance() 
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