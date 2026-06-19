#include <stdio.h>

#include "decoder.h"

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    // Tell the assembler we are assembling for a 16 bit CPU so that the output
    // can be reassembled and automatically tested against the reference
    // assembly using a diff tool.
    printf("bits 16\n\n");

    char *path = argv[1];
    FILE *fptr = fopen(path, "rb");
    if (fptr == NULL) {
        printf("Unable to open file '%s'", path);
        return 1;
    }

    decode_file(fptr);

    fclose(fptr);
    return 0;
}
