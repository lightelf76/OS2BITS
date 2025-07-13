#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LX_UNPACK_IMPLEMENTATION
#include "lxunpack.h"

FILE *fi = NULL;
FILE *fo = NULL;

uint8_t pak[LX_PAGE_SIZE] = {0};
uint8_t unp[LX_PAGE_SIZE] = {0};

int main(int argc, char *argv[])
{
    int32_t flen, ulen, mode;
    int rc = 0;

    if (argc < 4) 
    {
        fprintf(stdout, "USAGE: %s <mode: 1 or 2> <input file> <output file>\n", argv[0]);
        rc = 1;
        goto end;
    }
    mode = atoi(argv[1]);
    if ((mode != 1) && (mode != 2))
    {
        fprintf(stderr, "Wrong unpacking mode %s, must be 1 or 2!\n", argv[1]);
        rc = 1;
        goto end;
    }
    fi = fopen(argv[2], "rb");
    if (!fi)
    {
        fprintf(stderr, "Error to open file %s\n", argv[2]);
        rc = 2;
        goto end;
    }
    fo = fopen(argv[3], "wb");
    if (!fo)
    {
        fprintf(stderr, "Error to create file %s\n", argv[3]);
        rc = 3;
        goto end;
    }
    fseek(fi, 0, SEEK_END);
    flen = ftell(fi);
    fseek(fi, 0, SEEK_SET);
    if (flen > LX_PAGE_SIZE)
    {
        flen = LX_PAGE_SIZE;
    }
    if (flen != fread(pak, 1, flen, fi))
    {
        fprintf(stderr, "Error to read file %s\n", argv[2]);
        rc = 4;
        goto end;
    }
    if (mode == 2)
    {
        ulen = lx_unpack2(unp, pak, flen);
    }
    else
    {
        ulen = lx_unpack1(unp, pak, flen);
    }
    if (ulen > 0)
    {
        fprintf(stdout, "Unpacking %d from %d bytes succeed\n", ulen, flen);
    }
    else
    {
        fprintf(stdout, "Unpacking %d bytes failed after %d bytes\n", flen, -ulen);
    }
    fflush(stdout);
    if (ulen != fwrite(unp, 1, ulen, fo))
    {
        fprintf(stderr, "Error to write file %s\n", argv[3]);
        rc = 5;
        goto end;
    }
    fprintf(stdout, "Done!\n");
    fflush(stdout);

end:
    if (fi) fclose(fi);
    if (fo) fclose(fo);
    return rc;
}
