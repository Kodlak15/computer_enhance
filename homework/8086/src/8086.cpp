#include <cassert>
#include <stdio.h>

// https://codeberg.org/bolt/8086-Users-Manual/src/branch/main/INTEL_The-8086-Family-Users-Manual.pdf
// See page 262

// Useful for debugging
void print_byte_as_bits(char byte) {
    for (int i = 7; 0 <= i; i--) {
        printf("%c", (byte & (1 << i)) ? '1' : '0');
    }
}

const char *decode_register(char byte, char w) {
    switch (byte) {
    case 0b000:
        return w == 0 ? "al" : "ax";
    case 0b001:
        return w == 0 ? "cl" : "cx";
    case 0b010:
        return w == 0 ? "dl" : "dx";
    case 0b011:
        return w == 0 ? "bl" : "bx";
    case 0b100:
        return w == 0 ? "ah" : "sp";
    case 0b101:
        return w == 0 ? "ch" : "bp";
    case 0b110:
        return w == 0 ? "dh" : "si";
    case 0b111:
        return w == 0 ? "bh" : "di";
    }

    return nullptr;
}

// Decode the binary instructions inside the file at the specified path.
int decode_instructions(char *path) {
    FILE *fptr = fopen(path, "r");
    if (fptr == nullptr) {
        printf("Unable to open file at `%s`\n", path);
        return 1;
    }

    printf("bits 16\n\n");

    char byte1;
    while ((byte1 = fgetc(fptr)) != EOF) {
        // Register to register move
        if (((byte1 >> 2) & 0b00111111) == 0b100010) {
            // the d bit tells us which direction the data moves
            char d = byte1 & 0b00000010;
            // the w bit tells us whether we need an 8 bit or 16 bit register
            char w = byte1 & 0b00000001;

            // Get the next byte
            char byte2 = fgetc(fptr);

            // Mode field
            char mod = (byte2 >> 6) & 0b00000011;

            char reg = (byte2 >> 3) & 0b00000111;
            char rm = byte2 & 0b00000111;

            // Assign the source and destination registers
            const char *src = (d == 0) ? decode_register(reg, w) : decode_register(rm, w);
            if (src == nullptr) {
                printf("Unable to decode source register\n");
                return 1;
            }
            const char *dst = (d == 0) ? decode_register(rm, w) : decode_register(reg, w);
            if (src == nullptr) {
                printf("Unable to decode destination register\n");
                return 1;
            }

            // Print the decoded instruction
            printf("mov %s, %s\n", dst, src);
        }
    }
    fclose(fptr);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    return decode_instructions(argv[1]);
}
