#include "decoder.h"
#include "simulator.h"
#include <stdio.h>
#include <string.h>

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    if (argc == 2) {
        char *path = argv[1];
        FILE *fptr = fopen(path, "rb");
        if (fptr == NULL) {
            printf("Unable to open file '%s'", path);
            return 1;
        }

        printf("bits 16\n\n");
        decode_8086(fptr);
        fclose(fptr);
    } else if (strcmp(argv[1], "--exec") == 0) {
        // TODO
        char *path = argv[2];
        FILE *fptr = fopen(path, "rb");
        if (fptr == NULL) {
            printf("Unable to open file '%s'", path);
            return 1;
        }
        simulate_8086(fptr);
        fclose(fptr);
    } else {
        printf("Invalid argument: %s\n", argv[1]);
    }

    return 0;
}
