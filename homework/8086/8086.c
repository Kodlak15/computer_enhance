#include "8086_decoder.h"

#include <stdio.h>

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    const char *path = argv[1];

    FileContents file = read_file(path);
    if (!file.bytes) {
        return 1;
    }

    DecoderResult decoded_result = decode(file);

    // todo: separate decoder/simulator logic
    printf("bits 16\n\n");
    for (int i = 0; i < decoded_result.count; i++) {
        print_instruction(decoded_result.instructions[i]);
    }

    return 0;
}
