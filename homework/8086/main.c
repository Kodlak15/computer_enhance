#include <stdint.h>
#include <stdio.h>

void disassemble_file(FILE *fptr);
const char *decode_reg(unsigned char reg, unsigned char w);
void decode_effective_address(char buf[], char rm, int16_t disp);

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

// Register/memory to/from register
// 100010dw - mov
// 000000dw - add
// 001010dw - sub
// 001110dw - cmp

// Immediate to register/memory
// 1100011w - mov
// 100000sw - add
// 100000sw - sub
// 100000sw - cmp

// Memory to accumulator
// 1010000w - mov
// 0000010w - add
// 0010110w - sub
// 0011110w - cmp

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

    disassemble_file(fptr);

    fclose(fptr);
    return 0;
}

void disassemble_file(FILE *fptr) {
    int b1;
    while ((b1 = fgetc(fptr)) != EOF) {
        if (0 == 0) {
            // Register/memory to/from register
            const char *op;
            if ((b1 & 0b10101000) == 0b10001000) {
                op = "mov";
            } else if (((b1 >> 3) & 0b111) == 0b000) {
                op = "add";
            } else if (((b1 >> 3) & 0b111) == 0b101) {
                op = "sub";
            } else if (((b1 >> 3) & 0b111) == 0b111) {
                op = "cmp";
            }

            unsigned char d = (b1 >> 1) & 0b00000001;
            unsigned char w = b1 & 0b00000001;
            int b2 = fgetc(fptr);
            unsigned char mod = (b2 >> 6) & 0b00000011;
            unsigned char reg = (b2 >> 3) & 0b00000111;
            unsigned char rm = b2 & 0b00000111;

            char rm_buf[32];
            switch (mod) {
                case 0b00: {
                    if (rm == 0b110) {
                        int disp_lo = fgetc(fptr);
                        int disp_hi = fgetc(fptr);
                        int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));
                        snprintf(rm_buf, sizeof(rm_buf), "[%d]", disp);
                    } else {
                        decode_effective_address(rm_buf, rm, 0);
                    }
                } break;
                case 0b01: {
                    int16_t disp = (int8_t)fgetc(fptr);
                    decode_effective_address(rm_buf, rm, disp);
                } break;
                case 0b10: {
                    int disp_lo = fgetc(fptr);
                    int disp_hi = fgetc(fptr);
                    int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));
                    decode_effective_address(rm_buf, rm, disp);
                } break;
                case 0b11: {
                    snprintf(rm_buf, sizeof(rm_buf), "%s", decode_reg(rm, w));
                } break;
            }

            const char *src;
            const char *dst;
            if (d == 0) {
                src = decode_reg(reg, w);
                dst = rm_buf;
            } else {
                src = rm_buf;
                dst = decode_reg(reg, w);
            }

            printf("%s %s, %s\n", op, dst, src);
        } else if (0 == 0) {
            // Immediate to register/memory
        } else if (0 == 0) {
            // Memory to accumulator
        } else if (0 == 0) {
            // Immediate to register
            unsigned char w = (b1 >> 3) & 0b00000001;
            unsigned char reg = b1 & 0b00000111;
            int data_lo = fgetc(fptr);

            const char *src;
            int16_t data;
            if (w == 0) {
                src = decode_reg(reg, 0);
                data = (int8_t)data_lo;
            } else {
                int data_hi = fgetc(fptr);
                src = decode_reg(reg, 1);
                data = (int16_t)((int16_t)data_lo | ((int16_t)data_hi << 8));
            }

            printf("mov %s, %d\n", src, data);
        }
    }
}

// void disassemble_file(FILE *fptr) {
//     int b1;
//     while ((b1 = fgetc(fptr)) != EOF) {
//         if (((b1 >> 2) & 0b00111111) == 0b100010) {
//             // Register/memory to/from register
//             unsigned char d = (b1 >> 1) & 0b00000001;
//             unsigned char w = b1 & 0b00000001;
//             int b2 = fgetc(fptr);
//             unsigned char mod = (b2 >> 6) & 0b00000011;
//             unsigned char reg = (b2 >> 3) & 0b00000111;
//             unsigned char rm = b2 & 0b00000111;
//
//             char rm_buf[32];
//             switch (mod) {
//                 case 0b00: {
//                     if (rm == 0b110) {
//                         int disp_lo = fgetc(fptr);
//                         int disp_hi = fgetc(fptr);
//                         int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));
//                         snprintf(rm_buf, sizeof(rm_buf), "[%d]", disp);
//                     } else {
//                         decode_effective_address(rm_buf, rm, 0);
//                     }
//                 } break;
//                 case 0b01: {
//                     int16_t disp = (int8_t)fgetc(fptr);
//                     decode_effective_address(rm_buf, rm, disp);
//                 } break;
//                 case 0b10: {
//                     int disp_lo = fgetc(fptr);
//                     int disp_hi = fgetc(fptr);
//                     int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));
//                     decode_effective_address(rm_buf, rm, disp);
//                 } break;
//                 case 0b11: {
//                     snprintf(rm_buf, sizeof(rm_buf), "%s", decode_reg(rm, w));
//                 } break;
//             }
//
//             const char *src;
//             const char *dst;
//             if (d == 0) {
//                 src = decode_reg(reg, w);
//                 dst = rm_buf;
//             } else {
//                 src = rm_buf;
//                 dst = decode_reg(reg, w);
//             }
//
//             printf("mov %s, %s\n", dst, src);
//         } else if (((b1 >> 4) & 0b00001111) == 0b00001011) {
//             // Immediate to register
//             unsigned char w = (b1 >> 3) & 0b00000001;
//             unsigned char reg = b1 & 0b00000111;
//             int data_lo = fgetc(fptr);
//
//             const char *src;
//             int16_t data;
//             if (w == 0) {
//                 src = decode_reg(reg, 0);
//                 data = (int8_t)data_lo;
//             } else {
//                 int data_hi = fgetc(fptr);
//                 src = decode_reg(reg, 1);
//                 data = (int16_t)((int16_t)data_lo | ((int16_t)data_hi << 8));
//             }
//
//             printf("mov %s, %d\n", src, data);
//         }
//     }
// }

const char *decode_reg(unsigned char reg, unsigned char w) {
    switch (reg) {
        case 0b000: {
            return w == 0 ? "al" : "ax";
        } break;
        case 0b001: {
            return w == 0 ? "cl" : "cx";
        } break;
        case 0b010: {
            return w == 0 ? "dl" : "dx";
        }; break;
        case 0b011: {
            return w == 0 ? "bl" : "bx";
        } break;
        case 0b100: {
            return w == 0 ? "ah" : "sp";
        } break;
        case 0b101: {
            return w == 0 ? "ch" : "bp";
        } break;
        case 0b110: {
            return w == 0 ? "dh" : "si";
        } break;
        case 0b111: {
            return w == 0 ? "bh" : "di";
        } break;
    }

    return 0;
}

void decode_effective_address(char buf[], char rm, int16_t disp) {
    switch (rm) {
        case 0b000: {
            if (disp == 0) {
                snprintf(buf, 32, "[bx + si]");
            } else {
                snprintf(buf, 32, "[bx + si + %d]", disp);
            }
        } break;
        case 0b001: {
            if (disp == 0) {
                snprintf(buf, 32, "[bx + di]");
            } else {
                snprintf(buf, 32, "[bx + di + %d]", disp);
            }
        } break;
        case 0b010: {
            if (disp == 0) {
                snprintf(buf, 32, "[bp + si]");
            } else {
                snprintf(buf, 32, "[bp + si + %d]", disp);
            }
        } break;
        case 0b011: {
            if (disp == 0) {
                snprintf(buf, 32, "[bp + di]");
            } else {
                snprintf(buf, 32, "[bp + di + %d]", disp);
            }
        } break;
        case 0b100: {
            if (disp == 0) {
                snprintf(buf, 32, "[si]");
            } else {
                snprintf(buf, 32, "[si + %d]", disp);
            }
        } break;
        case 0b101: {
            if (disp == 0) {
                snprintf(buf, 32, "[di]");
            } else {
                snprintf(buf, 32, "[di + %d]", disp);
            }
        } break;
        case 0b110: {
            if (disp == 0) {
                snprintf(buf, 32, "[bp]");
            } else {
                snprintf(buf, 32, "[bp + %d]", disp);
            }
        } break;
        case 0b111: {
            if (disp == 0) {
                snprintf(buf, 32, "[bx]");
            } else {
                snprintf(buf, 32, "[bx + %d]", disp);
            }
        } break;
    }
}
