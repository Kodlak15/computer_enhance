#include <stdio.h>

int disassemble(char *path);
const char *decode_register(char reg, char w);
const char *decode_effective_address(char rm, char mod, FILE *fptr);

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

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

    char b1;
    while ((b1 = fgetc(fptr)) != EOF) {
        // Register/memory to/from register
        if (((b1 >> 2) & 0b00111111) == 0b100010) {
            char d = b1 & 0b00000010;
            char w = b1 & 0b00000001;
            char b2 = fgetc(fptr);
            char mod = (b2 >> 6) & 0b00000011;
            char reg = (b2 >> 3) & 0b00000111;
            char rm = b2 & 0b00000111;

            const char *src;
            const char *dst;

            if (mod == 0b11) {
                if (d == 0) {
                    src = decode_register(reg, w);
                    dst = decode_register(rm, w);
                } else {
                    src = decode_register(rm, w);
                    dst = decode_register(reg, w);
                }
            } else {
                if (d == 0) {
                    src = decode_register(reg, w);
                    dst = decode_effective_address(rm, mod, fptr);
                } else {
                    src = decode_register(reg, w);
                    dst = decode_effective_address(rm, mod, fptr);
                }
            }

            printf("mov %s, %s\n", dst, src);
        }

        // Immediate to register/memory
    }

    fclose(fptr);
    return 0;
}

const char *decode_register(char reg, char w) {
    switch (reg) {
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

const char *decode_effective_address(char rm, char mod, FILE *fptr) {
    static char buf[16];

    switch (mod) {
        case 0b00: {
            switch (rm) {
                case 0b000: {
                    sprintf(buf, "[bx + si]");
                }
                case 0b001: {
                    sprintf(buf, "[bx + di]");
                }
                case 0b010: {
                    sprintf(buf, "[bp + si]");
                }
                case 0b011: {
                    sprintf(buf, "[bp + di]");
                }
                case 0b100: {
                    sprintf(buf, "[si]");
                }
                case 0b101: {
                    sprintf(buf, "[di]");
                }
                case 0b110: {
                    sprintf(buf, "%d", rm);
                }
                case 0b111: {
                    sprintf(buf, "[bx]");
                }
            }
        };
        case 0b01: {
            char dlo = fgetc(fptr);
            switch (rm) {
                case 0b000: {
                    sprintf(buf, "[bx + si + %d]", dlo);
                }
                case 0b001: {
                    sprintf(buf, "[bx + di + %d]", dlo);
                }
                case 0b010: {
                    sprintf(buf, "[bp + si + %d]", dlo);
                }
                case 0b011: {
                    sprintf(buf, "[bp + di + %d]", dlo);
                }
                case 0b100: {
                    sprintf(buf, "[si + %d]", dlo);
                }
                case 0b101: {
                    sprintf(buf, "[di + %d]", dlo);
                }
                case 0b110: {
                    sprintf(buf, "[bp + %d]", dlo);
                }
                case 0b111: {
                    sprintf(buf, "[bx + %d]", dlo);
                }
            };
        }
        case 0b10: {
            char dlo = fgetc(fptr);
            char dhi = fgetc(fptr);
            switch (rm) {
                case 0b000: {
                    sprintf(buf, "[bx + si + %d]", dlo + dhi);
                }
                case 0b001: {
                    sprintf(buf, "[bx + di + %d]", dlo + dhi);
                }
                case 0b010: {
                    sprintf(buf, "[bp + si + %d]", dlo + dhi);
                }
                case 0b011: {
                    sprintf(buf, "[bp + di + %d]", dlo + dhi);
                }
                case 0b100: {
                    sprintf(buf, "[si + %d]", dlo + dhi);
                }
                case 0b101: {
                    sprintf(buf, "[di + %d]", dlo + dhi);
                }
                case 0b110: {
                    sprintf(buf, "[bp + %d]", dlo + dhi);
                }
                case 0b111: {
                    sprintf(buf, "[bx + %d]", dlo + dhi);
                }
            };
        };
    }

    return buf;
}
