#include <stdio.h>

#include "decoder.h"

void decode_file(FILE *fptr) {
    int b1;
    while ((b1 = fgetc(fptr)) != EOF) {
        if ((b1 & 0b11111100) == 0b10001000) {
            decode_rm_reg(fptr, b1, "mov");
        } else if ((b1 & 0b11110000) == 0b10110000) {
            decode_imm_reg(fptr, b1, "mov");
        } else if ((b1 & 0b11000100) == 0b00000000) {
            const char *mnemonic;
            if ((b1 & 0b00111000) == 0b00000000) {
                mnemonic = "add";
            } else if ((b1 & 0b00111000) == 0b00101000) {
                mnemonic = "sub";
            } else if ((b1 & 0b00111000) == 0b00111000) {
                mnemonic = "cmp";
            }

            decode_rm_reg(fptr, b1, mnemonic);
        } else if ((b1 & 0b11111100) == 0b10000000) {
            int b2 = fgetc(fptr);

            const char *mnemonic;
            if ((b2 & 0b00111000) == 0b00000000) {
                mnemonic = "add";
            } else if ((b2 & 0b00111000) == 0b00101000) {
                mnemonic = "sub";
            } else if ((b2 & 0b00111000) == 0b00111000) {
                mnemonic = "cmp";
            }

            decode_imm_rm(fptr, b1, b2, mnemonic);
        } else if ((b1 & 0b11000100) == 0b00000100) {
            const char *mnemonic;
            if ((b1 & 0b00111000) == 0b00000000) {
                mnemonic = "add";
            } else if ((b1 & 0b00111000) == 0b00101000) {
                mnemonic = "sub";
            } else if ((b1 & 0b00111000) == 0b00111000) {
                mnemonic = "cmp";
            }

            decode_imm_accum(fptr, b1, mnemonic);
        } else if ((b1 & 11110000) == 0b01110000) {
            const char *mnemonic;
            if ((b1 & 0b00001111) == 0b00000100) {
                mnemonic = "je";
            } else if ((b1 & 0b00001111) == 0b00001100) {
                mnemonic = "jl";
            } else if ((b1 & 0b00001111) == 0b00001110) {
                mnemonic = "jle";
            } else if ((b1 & 0b00001111) == 0b00000010) {
                mnemonic = "jb";
            } else if ((b1 & 0b00001111) == 0b00000110) {
                mnemonic = "jbe";
            } else if ((b1 & 0b00001111) == 0b00001010) {
                mnemonic = "jp";
            } else if ((b1 & 0b00001111) == 0b00000000) {
                mnemonic = "jo";
            } else if ((b1 & 0b00001111) == 0b00001000) {
                mnemonic = "js";
            } else if ((b1 & 0b00001111) == 0b00000101) {
                mnemonic = "jne";
            } else if ((b1 & 0b00001111) == 0b00001101) {
                mnemonic = "jnl";
            } else if ((b1 & 0b00001111) == 0b00001111) {
                mnemonic = "jg";
            } else if ((b1 & 0b00001111) == 0b00000011) {
                mnemonic = "jnb";
            } else if ((b1 & 0b00001111) == 0b00000111) {
                mnemonic = "ja";
            } else if ((b1 & 0b00001111) == 0b00001011) {
                mnemonic = "jnp";
            } else if ((b1 & 0b00001111) == 0b00000001) {
                mnemonic = "jno";
            } else if ((b1 & 0b00001111) == 0b00001001) {
                mnemonic = "jns";
            }

            decode_cond_jmp(fptr, mnemonic);
        } else if ((b1 & 0b11111100) == 0b11100000) {
            const char *mnemonic;
            if ((b1 & 0b00000011) == 0b00000010) {
                mnemonic = "loop";
            } else if ((b1 & 0b00000011) == 0b00000001) {
                mnemonic = "loopz";
            } else if ((b1 & 0b00000011) == 0b00000000) {
                mnemonic = "loopnz";
            } else if ((b1 & 0b00000011) == 0b00000011) {
                mnemonic = "jcxz";
            }

            decode_loop(fptr, mnemonic);
        }
    }
}

const char *decode_reg(unsigned char reg, int is_word) {
    const char *r;
    switch (reg) {
        case 0b000: {
            r = is_word ? "ax" : "al";
        } break;
        case 0b001: {
            r = is_word ? "cx" : "cl";
        } break;
        case 0b010: {
            r = is_word ? "dx" : "dl";
        }; break;
        case 0b011: {
            r = is_word ? "bx" : "bl";
        } break;
        case 0b100: {
            r = is_word ? "sp" : "ah";
        } break;
        case 0b101: {
            r = is_word ? "bp" : "ch";
        } break;
        case 0b110: {
            r = is_word ? "si" : "dh";
        } break;
        case 0b111: {
            r = is_word ? "di" : "bh";
        } break;
    }

    return r;
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

void decode_rm_reg(FILE *fptr, int b1, const char *mnemonic) {
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
                decode_effective_address(rm_buf, rm, disp);
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

    printf("%s %s, %s\n", mnemonic, dst, src);
}

void decode_imm_reg(FILE *fptr, int b1, const char *mnemonic) {
    unsigned char w = (b1 >> 3) & 0b00000001;
    unsigned char reg = b1 & 0b00000111;

    int data_lo = fgetc(fptr);
    int16_t data;
    if (w == 1) {
        int data_hi = fgetc(fptr);
        data = (int16_t)((int16_t)data_lo | ((int16_t)data_hi << 8));
    } else {
        data = (int8_t)data_lo;
    }

    const char *dst = decode_reg(reg, w);
    printf("%s %s, %d\n", mnemonic, dst, data);
}

void decode_imm_rm(FILE *fptr, int b1, int b2, const char *mnemonic) {
    unsigned char s = (b1 >> 1) & 0b00000001;
    unsigned char w = b1 & 0b00000001;
    unsigned char mod = (b2 >> 6) & 0b00000011;
    unsigned char rm = b2 & 0b00000111;

    char rm_buf[32];
    switch (mod) {
        case 0b00: {
            if (rm == 0b110) {
                int disp_lo = fgetc(fptr);
                int disp_hi = fgetc(fptr);
                int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));
                decode_effective_address(rm_buf, rm, disp);
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

    int data_lo = fgetc(fptr);
    int16_t data;
    if (s == 0 && w == 1) {
        int data_hi = fgetc(fptr);
        data = (int16_t)((int16_t)data_lo | ((int16_t)data_hi << 8));
    } else {
        data = (int8_t)data_lo;
    }

    const char *dst = rm_buf;
    if (mod == 0b11) {
        printf("%s %s, %d\n", mnemonic, dst, data);
    } else if (w == 1) {
        printf("%s word %s, %d\n", mnemonic, dst, data);
    } else {
        printf("%s byte %s, %d\n", mnemonic, dst, data);
    }
}

void decode_imm_accum(FILE *fptr, int b1, const char *mnemonic) {
    unsigned char w = b1 & 0b00000001;

    const char *dst;
    int data_lo = fgetc(fptr);
    int16_t data;
    if (w == 1) {
        dst = "ax";
        int data_hi = fgetc(fptr);
        data = (int16_t)((int16_t)data_lo | ((int16_t)data_hi << 8));
    } else {
        dst = "al";
        data = (int8_t)data_lo;
    }

    printf("%s %s, %d\n", mnemonic, dst, data);
}

void decode_cond_jmp(FILE *fptr, const char *mnemonic) {
    int b2 = fgetc(fptr);
    int8_t inc = (int8_t)b2;
    printf("%s %d\n", mnemonic, inc);
}

void decode_loop(FILE *fptr, const char *mnemonic) {
    int b2 = fgetc(fptr);
    int8_t inc = (int8_t)b2;
    printf("%s %d\n", mnemonic, inc);
}
