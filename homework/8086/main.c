#include <stdint.h>
#include <stdio.h>

void disassemble_file(FILE *fptr);
const char *decode_reg(unsigned char reg, int is_word);
const char *decode_effective_address(char rm);

// See page 161:
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

// Register/memory to/from register
// 100010dw - mov
// 000000dw - add
// 001010dw - sub
// 001110dw - cmp
// 11000100 - mask

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
        if ((b1 & 0b11111100) == 0b10001000) {
            // MOV register/memory to/from register
            //
            // decode_rm_reg(fptr, b1, mnemonic) -> todo
        } else if ((b1 & 0b11000100) == 0b00000000) {
            // ALU register/memory with register to either
            //
            // const char *mnemonic;
            // if (Bits 3-5 == ADD pattern) {
            //     mnemonic = "add";
            // } else if (Bits 3-5 == SUB pattern) {
            //     mnemonic = "sub";
            // } else if (Bits 3-5 == CMP pattern) {
            //     mnemonic = "cmp";
            // }
            //
            // decode_rm_reg(fptr, b1, mnemonic) -> todo
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

const char *decode_effective_address(char rm) {
    const char *ea;
    switch (rm) {
        case 0b000: {
            ea = "bx + si";
        } break;
        case 0b001: {
            ea = "bx + di";
        } break;
        case 0b010: {
            ea = "bp + si";
        } break;
        case 0b011: {
            ea = "bp + di";
        } break;
        case 0b100: {
            ea = "si";
        } break;
        case 0b101: {
            ea = "di";
        } break;
        case 0b110: {
            ea = "bp";
        } break;
        case 0b111: {
            ea = "bx";
        } break;
    }

    return ea;
}
