// 8086 Manual (see page 161):
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef uint8_t u8;
typedef uint16_t u16;

typedef int8_t i8;
typedef int16_t i16;

struct ByteArray {
    u8 *buf;
    size_t size;
};

ByteArray ByteArrayFromFile(FILE *file, size_t size) {
    ByteArray a;
    a.buf = nullptr;

    u8 *buf = (u8 *)malloc(size);
    if (!buf) {
        fprintf(stderr, "Error: Failed to allocate memory for file contents.\n");
        fclose(file);
        return a;
    }

    size_t readCount = fread(buf, 1, size, file);
    fclose(file);

    if (readCount != (size_t)size) {
        fprintf(stderr, "Error: Short read on the chosen file.\n");
        return a;
    }

    a.buf = buf;
    a.size = size;

    return a;
}

enum Mnemonic : u8 {
    MNEMONIC_MOV,
    MNEMONIC_ADD,
    MNEMONIC_SUB,
    MNEMONIC_CMP,
    MNEMONIC_JE,
    MNEMONIC_JL,
    MNEMONIC_JB,
    MNEMONIC_JBE,
    MNEMONIC_JP,
    MNEMONIC_JO,
    MNEMONIC_JS,
    MNEMONIC_JNE,
    MNEMONIC_JNL,
    MNEMONIC_JG,
    MNEMONIC_JNB,
    MNEMONIC_JA,
    MNEMONIC_JNP,
    MNEMONIC_JNO,
    MNEMONIC_JNS,
    MNEMONIC_JLE,
    MNEMONIC_JNLE,
    MNEMONIC_LOOP,
    MNEMONIC_LOOPZ,
    MNEMONIC_LOOPNZ,
    MNEMONIC_JCXZ,
};

const char *MnemonicAsString(Mnemonic m) {
    switch (m) {
        case MNEMONIC_MOV:
            return "mov";
        case MNEMONIC_ADD:
            return "add";
        case MNEMONIC_SUB:
            return "sub";
        case MNEMONIC_CMP:
            return "cmp";
        case MNEMONIC_JE:
            return "je";
        case MNEMONIC_JL:
            return "jl";
        case MNEMONIC_JB:
            return "jb";
        case MNEMONIC_JBE:
            return "jbe";
        case MNEMONIC_JP:
            return "jp";
        case MNEMONIC_JO:
            return "jo";
        case MNEMONIC_JS:
            return "js";
        case MNEMONIC_JNE:
            return "jne";
        case MNEMONIC_JNL:
            return "jnl";
        case MNEMONIC_JG:
            return "jg";
        case MNEMONIC_JNB:
            return "jnb";
        case MNEMONIC_JA:
            return "ja";
        case MNEMONIC_JNP:
            return "jnp";
        case MNEMONIC_JNO:
            return "jno";
        case MNEMONIC_JNS:
            return "jns";
        case MNEMONIC_JLE:
            return "jle";
        case MNEMONIC_JNLE:
            return "jnle";
        case MNEMONIC_LOOP:
            return "loop";
        case MNEMONIC_LOOPZ:
            return "loopz";
        case MNEMONIC_LOOPNZ:
            return "loopnz";
        case MNEMONIC_JCXZ:
            return "jcxz";
    }
}

enum Register : u8 {
    REGISTER_AL,
    REGISTER_AX,
    REGISTER_CL,
    REGISTER_CX,
    REGISTER_DL,
    REGISTER_DX,
    REGISTER_BL,
    REGISTER_BX,
    REGISTER_AH,
    REGISTER_SP,
    REGISTER_CH,
    REGISTER_BP,
    REGISTER_DH,
    REGISTER_SI,
    REGISTER_BH,
    REGISTER_DI,
    REGISTER_NULL,
};

const char *RegisterAsString(Register reg) {
    switch (reg) {
        case REGISTER_AL:
            return "al";
        case REGISTER_AX:
            return "ax";
        case REGISTER_CL:
            return "cl";
        case REGISTER_CX:
            return "cx";
        case REGISTER_DL:
            return "dl";
        case REGISTER_DX:
            return "dx";
        case REGISTER_BL:
            return "bl";
        case REGISTER_BX:
            return "bx";
        case REGISTER_AH:
            return "ah";
        case REGISTER_SP:
            return "sp";
        case REGISTER_CH:
            return "ch";
        case REGISTER_BP:
            return "bp";
        case REGISTER_DH:
            return "dh";
        case REGISTER_SI:
            return "si";
        case REGISTER_BH:
            return "bh";
        case REGISTER_DI:
            return "di";
        case REGISTER_NULL:
            return "";
    }
}

struct EffectiveAddress {
    Register reg1;
    Register reg2;
    i16 disp;
};

const char *EffectiveAddressAsString(char buf[], EffectiveAddress ea) {
    if (ea.reg1 == REGISTER_NULL) {
        sprintf(buf, "[%d]", ea.disp);
    } else if (ea.reg2 == REGISTER_NULL) {
        sprintf(buf, "[%s + %d]", RegisterAsString(ea.reg1), ea.disp);
    } else {
        sprintf(buf, "[%s + %s + %d]", RegisterAsString(ea.reg1), RegisterAsString(ea.reg2), ea.disp);
    }

    return buf;
}

enum OperandType {
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_IMMEDIATE,
    OPERAND_TYPE_MEMORY,
};

struct Operand {
    OperandType type;
    union {
        Register reg;
        EffectiveAddress eAddr;
        i16 imm;
    };
};

// Decode a register operand using the reg and w fields.
Operand DecodeReg(u8 reg, u8 w) {
    Operand o;
    o.type = OPERAND_TYPE_REGISTER;

    switch (reg) {
        case 0x00:
            o.reg = (w == 0) ? REGISTER_AL : REGISTER_AX;
            break;
        case 0x01: {
            o.reg = (w == 0) ? REGISTER_CL : REGISTER_CX;
            break;
        };
        case 0x02: {
            o.reg = (w == 0) ? REGISTER_DL : REGISTER_DX;
            break;
        };
        case 0x03: {
            o.reg = (w == 0) ? REGISTER_BL : REGISTER_BX;
            break;
        };
        case 0x04: {
            o.reg = (w == 0) ? REGISTER_AH : REGISTER_SP;
            break;
        };
        case 0x05: {
            o.reg = (w == 0) ? REGISTER_CH : REGISTER_BP;
            break;
        };
        case 0x06: {
            o.reg = (w == 0) ? REGISTER_DH : REGISTER_SI;
            break;
        };
        case 0x07: {
            o.reg = (w == 0) ? REGISTER_BH : REGISTER_DI;
            break;
        };
        default: {
            o.reg = REGISTER_NULL;
            break;
        };
    }

    return o;
}

Operand DecodeRegMem(ByteArray *a, u8 rm, u8 mod, u8 w) {}

Operand DecodeImmediate() {}

struct Instruction {
    Mnemonic mnemonic;
    Operand src;
    Operand dst;
};

typedef Instruction (*InstructionHandler)(ByteArray *a, size_t offset, size_t *consumed);

Instruction MovRmReg(ByteArray *a, size_t offset, size_t *consumed) {
    Instruction instruction;
    instruction.mnemonic = MNEMONIC_MOV;

    u8 b1 = a->buf[offset++];
    *consumed += 1;
    u8 b2 = a->buf[offset++];
    *consumed += 1;

    u8 d = (b1 >> 1) & 0x01;
    u8 w = b1 & 0x01;
    u8 mod = (b2 >> 6) & 0x03;
    u8 reg = (b2 >> 3) & 0x07;
    u8 rm = b2 & 0x07;

    Operand rm_decoded = DecodeRegMem(a, rm, mod, w);
    Operand reg_decoded = DecodeReg(reg, w);

    if (d == 0) {
        instruction.operands[0] = reg_decoded;
        instruction.operands[1] = rm_decoded;
    } else {
        instruction.operands[0] = rm_decoded;
        instruction.operands[1] = reg_decoded;
    }

    return instruction;
}

InstructionHandler handlerTable[] = {
    //     // Add: Reg/memory with register to either
    //     [0x00] = add_sub_cmp_rm_reg,
    //     [0x01] = add_sub_cmp_rm_reg,
    //     [0x02] = add_sub_cmp_rm_reg,
    //     [0x03] = add_sub_cmp_rm_reg,
    //     // Add: Immediate to accumulator
    //     [0x04] = add_sub_cmp_imm_accum,
    //     [0x05] = add_sub_cmp_imm_accum,
    //     // Sub: Reg/memory and register to either
    //     [0x28] = add_sub_cmp_rm_reg,
    //     [0x29] = add_sub_cmp_rm_reg,
    //     [0x2a] = add_sub_cmp_rm_reg,
    //     [0x2b] = add_sub_cmp_rm_reg,
    //     // Sub: Immediate to accumulator
    //     [0x2c] = add_sub_cmp_imm_accum,
    //     [0x2d] = add_sub_cmp_imm_accum,
    //     // Cmp: Register/memory and register
    //     [0x38] = add_sub_cmp_rm_reg,
    //     [0x39] = add_sub_cmp_rm_reg,
    //     [0x3a] = add_sub_cmp_rm_reg,
    //     [0x3b] = add_sub_cmp_rm_reg,
    //     // Cmp: Immediate with accumulator
    //     [0x3c] = add_sub_cmp_imm_accum,
    //     [0x3d] = add_sub_cmp_imm_accum,
    //     // Add/Sub/Cmp: Immediate to/from/with register/memory (all have same first byte)
    //     [0x80] = add_sub_cmp_imm_rm,
    //     [0x81] = add_sub_cmp_imm_rm,
    //     [0x82] = add_sub_cmp_imm_rm,
    //     [0x83] = add_sub_cmp_imm_rm,
    // Move: Register/memory to/from register
    [0x88] = mov_rm_reg,
    [0x89] = mov_rm_reg,
    [0x8a] = mov_rm_reg,
    [0x8b] = mov_rm_reg,
    //     // Move: Immediate to register
    //     [0xb0] = mov_imm_reg,
    //     [0xb1] = mov_imm_reg,
    //     [0xb2] = mov_imm_reg,
    //     [0xb3] = mov_imm_reg,
    //     [0xb4] = mov_imm_reg,
    //     [0xb5] = mov_imm_reg,
    //     [0xb6] = mov_imm_reg,
    //     [0xb7] = mov_imm_reg,
    //     [0xb8] = mov_imm_reg,
    //     [0xb9] = mov_imm_reg,
    //     [0xba] = mov_imm_reg,
    //     [0xbb] = mov_imm_reg,
    //     [0xbc] = mov_imm_reg,
    //     [0xbd] = mov_imm_reg,
    //     [0xbe] = mov_imm_reg,
    //     [0xbf] = mov_imm_reg,
    //     // Jump: Unconditional jump
    //     [0x70] = cond_jmp,
    //     [0x71] = cond_jmp,
    //     [0x72] = cond_jmp,
    //     [0x73] = cond_jmp,
    //     [0x74] = cond_jmp,
    //     [0x75] = cond_jmp,
    //     [0x76] = cond_jmp,
    //     [0x77] = cond_jmp,
    //     [0x78] = cond_jmp,
    //     [0x79] = cond_jmp,
    //     [0x7a] = cond_jmp,
    //     [0x7b] = cond_jmp,
    //     [0x7c] = cond_jmp,
    //     [0x7d] = cond_jmp,
    //     [0x7e] = cond_jmp,
    //     [0x7f] = cond_jmp,
    //     // Loop: Loop instructions
    //     [0xe0] = loop,
    //     [0xe1] = loop,
    //     [0xe2] = loop,
    //     [0xe3] = loop,
};

Instruction *DecodeInstructions(ByteArray *a) {
    Instruction *instructions = (Instruction *)malloc(a->size * sizeof(Instruction));

    size_t offset = 0;
    size_t count = 0;
    size_t consumed;
    while (offset < a->size) {
        consumed = 0;
        instructions[count++] = handlerTable[a->buf[offset]](a, offset, &consumed);
        offset += consumed;
    }

    return instructions;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing argument [path]\n");
        return 1;
    }

    const char *path = argv[1];

    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "Could not compute the size of the file %s.\n", path);
        fclose(file);
        return 1;
    }

    ByteArray a = ByteArrayFromFile(file, size);
    if (!a.buf) {
        return 1;
    }
    printf("%s\n", a.buf);

    // todo: should be option to simulate
    DecodeInstructions(&a);

    // Release the byte array buffer (technically not necessary as the OS will clean it up, but for the sake of
    // completeness we do it anyways).
    free(a.buf);
}
