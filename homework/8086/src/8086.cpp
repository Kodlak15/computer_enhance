#include <stdio.h>

// https://codeberg.org/bolt/8086-Users-Manual/src/branch/main/INTEL_The-8086-Family-Users-Manual.pdf
// See page 262

void print_byte_as_bits(char byte) {
    for (int i = 7; 0 <= i; i--) {
        printf("%c", (byte & (1 << i)) ? '1' : '0');
    }
}

int decode_instructions(char *path) {
    FILE *file = fopen(path, "r");
    if (file == nullptr) {
        printf("Unable to open file at `%s`\n", path);
        return 1;
    }

    char byte;
    while ((byte = fgetc(file)) != EOF) {
        print_byte_as_bits(byte);
        putchar('\n');
        // Instruction decoding here ...
    }
    fclose(file);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    return decode_instructions(argv[1]);
}
