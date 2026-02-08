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
        if (w == 0) {
            return "al";
        } else {
            return "ax";
        }
    case 0b001:
        if (w == 0) {
            return "cl";
        } else {
            return "cx";
        }
    case 0b010:
        if (w == 0) {
            return "dl";
        } else {
            return "dx";
        }
    case 0b011:
        if (w == 0) {
            return "bl";
        } else {
            return "bx";
        }
    case 0b100:
        if (w == 0) {
            return "ah";
        } else {
            return "sp";
        }
    case 0b101:
        if (w == 0) {
            return "ch";
        } else {
            return "bp";
        }
    case 0b110:
        if (w == 0) {
            return "dh";
        } else {
            return "si";
        }
    case 0b111:
        if (w == 0) {
            return "bh";
        } else {
            return "di";
        }
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

            // The mod field tells us what kind of move this is
            char mod = (byte2 >> 6) & 0b00000011;

            char reg = (byte2 >> 3) & 0b00000111;
            char rm = byte2 & 0b00000111;

            // Decode the register ...
            const char *src;
            const char *dst;
            if (d == 0) {
                src = decode_register(reg, w);
                dst = decode_register(rm, w);
            } else {
                src = decode_register(rm, w);
                dst = decode_register(reg, w);
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
