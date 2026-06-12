#include <cstdint>
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

    printf("Unable to decode destination register\n");
    return nullptr;
}

const char *decode_memory(char byte, uint16_t disp) {
    switch (byte) {
    case 0b000: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[bx + si + %d]", disp);
            return buf;
        } else {
            return "[bx + si]";
        }
    } break;
    case 0b001: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[bx + di + %d]", disp);
            return buf;
        } else {
            return "[bx + di]";
        }
    } break;
    case 0b010: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[bp + si + %d]", disp);
            return buf;
        } else {
            return "[bp + si]";
        }
    } break;
    case 0b011: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[bp + di + %d]", disp);
            return buf;
        } else {
            return "[bp + di]";
        }
    } break;
    case 0b100: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[si + %d]", disp);
            return buf;
        } else {
            return "[si]";
        }
    } break;
    case 0b101: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[di + %d]", disp);
            return buf;
        } else {
            return "[di]";
        }
    } break;
    case 0b110: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[bp + %d]", disp);
            return buf;
        } else {
            return "[bp]";
        }
    } break;
    case 0b111: {
        if (disp > 0) {
            static char buf[32];
            sprintf(buf, "[bx + %d]", disp);
            return buf;
        } else {
            return "[bx]";
        }
    } break;
    }

    printf("Unable to decode memory\n");
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
        if (((byte1 >> 2) & 0b00111111) == 0b100010) {
            // The d bit tells us which direction the data moves
            char d = byte1 & 0b00000010;
            // The w bit tells us whether we need an 8 bit or 16 bit register
            char w = byte1 & 0b00000001;

            // Read the next byte
            char byte2 = fgetc(fptr);

            // Mode field
            char mod = (byte2 >> 6) & 0b00000011;

            // Register field
            char reg = (byte2 >> 3) & 0b00000111;

            // Register/memory field
            char rm = byte2 & 0b00000111;

            // Register or memory location the data is moving from
            const char *src;
            // Register or memory location the data is moving to
            const char *dst;

            switch (mod) {
            case 0b00: {
                src = (d == 0) ? decode_register(reg, w) : decode_memory(rm, 0);
                if (src == nullptr) {
                    return 1;
                }
                dst = (d == 0) ? decode_memory(rm, 0) : decode_register(reg, w);
                if (dst == nullptr) {
                    return 1;
                }
            } break;
            case 0b01: {
                char disp_lo = fgetc(fptr);
                src = (d == 0) ? decode_register(reg, w) : decode_memory(rm, disp_lo);
                if (src == nullptr) {
                    return 1;
                }
                dst = (d == 0) ? decode_memory(rm, disp_lo) : decode_register(reg, w);
                if (dst == nullptr) {
                    return 1;
                }
            } break;
            case 0b10: {
                uint8_t disp_lo = fgetc(fptr);
                uint8_t disp_hi = fgetc(fptr);
                src = (d == 0) ? decode_register(reg, w)
                               : decode_memory(rm, (disp_hi << 8) | disp_lo);
                if (src == nullptr) {
                    return 1;
                }
                dst = (d == 0) ? decode_memory(rm, (disp_hi << 8) | disp_lo)
                               : decode_register(reg, w);
                if (dst == nullptr) {
                    return 1;
                }
            } break;
            case 0b11: {
                src = (d == 0) ? decode_register(reg, w) : decode_register(rm, w);
                if (src == nullptr) {
                    return 1;
                }
                dst = (d == 0) ? decode_register(rm, w) : decode_register(reg, w);
                if (dst == nullptr) {
                    return 1;
                }
            } break;
            }

            printf("mov %s, %s\n", dst, src);
        } else if (((byte1 >> 4) & 0b00001111) == 0b1011) {
            char w = (byte1 >> 3) & 0b00000001;
            char reg = byte1 & 0b00000111;

            const char *dst;
            if (w == 1) {
                int16_t data_lo = fgetc(fptr);
                int16_t data_hi = fgetc(fptr);
                int16_t src = (data_hi << 8) | data_lo;
                dst = decode_register(reg, 1);
                printf("mov %s, %d\n", dst, src);
            } else {
                int8_t data_lo = fgetc(fptr);
                dst = decode_register(reg, 0);
                printf("mov %s, %d\n", dst, data_lo);
            }
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
