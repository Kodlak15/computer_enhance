#include "sim86_lib.h"

#include <stdio.h>
#include <string.h>

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

typedef enum { Decode, Simulate } Mode;

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    Mode mode = Decode;
    const char *path;

    if (argc > 2) {
        if (strcmp(argv[1], "--exec") == 0) {
            mode = Simulate;
            path = argv[2];
        } else {
            path = argv[1];
        }
    }

    FileContents file = read_file(path);
    if (!file.bytes) {
        return 1;
    }

    switch (mode) {
        case Decode:
            // todo
            printf("Decoding 8086\n");
            break;
        case Simulate:
            // todo
            printf("Simulating 8086\n");
            break;
    }

    return 0;
}
