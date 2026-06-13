#include <stdio.h>

int disassemble(char *path);
const char *decode_register(char r, char w);

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Missing required argument: path\n");
        return 1;
    }

    char *path = argv[1];
    return disassemble(path);
}

int disassemble(char *path) {
    FILE *fptr = fopen(path, "r");
    if (fptr == NULL) {
        printf("Unable to open file '%s'", path);
        return 1;
    }

    // Tell the assembler we are assembling for a 16 bit CPU so that the output
    // can be reassembled and automatically tested against the reference
    // assembly using a diff tool.
    printf("bits 16\n\n");

    char b1, b2, d, w, mod, reg, rm;
    while ((b1 = fgetc(fptr)) != EOF) {
        if (((b1 >> 2) & 0b00111111) == 0b100010) {
            d = b1 & 0b00000010;
            w = b1 & 0b00000001;
            b2 = fgetc(fptr);
            mod = (b2 >> 6) & 0b00000011;
            reg = (b2 >> 3) & 0b00000111;
            rm = b2 & 0b00000111;

            const char *src;
            const char *dst;
            if (d == 0) {
                src = decode_register(reg, w);
                dst = decode_register(rm, w);
            } else {
                src = decode_register(rm, w);
                dst = decode_register(reg, w);
            }

            printf("mov %s, %s\n", dst, src);
        }
    }

    fclose(fptr);
    return 0;
}

const char *decode_register(char r, char w) {
    switch (r) {
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

    return 0;
}
