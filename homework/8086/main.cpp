// 8086 Manual (see page 161):
// https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

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

// Read the next byte from a buffer and increment the pointer.
u8 ReadByte(u8 **p) {
    return *(*p)++;
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

// Read the string representation of the effective address calculation into buf.
void EffectiveAddressAsString(char buf[], EffectiveAddress ea) {
    if (ea.reg1 == REGISTER_NULL) {
        sprintf(buf, "[%d]", ea.disp);
    } else if (ea.reg2 == REGISTER_NULL) {
        sprintf(buf, "[%s + %d]", RegisterAsString(ea.reg1), ea.disp);
    } else {
        sprintf(buf, "[%s + %s + %d]", RegisterAsString(ea.reg1), RegisterAsString(ea.reg2), ea.disp);
    }
}

enum OperandType {
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_EFFECTIVE_ADDRESS,
    OPERAND_TYPE_IMMEDIATE,
};

struct Operand {
    OperandType type;
    union {
        Register reg;
        EffectiveAddress eAddr;
        i16 imm;
    };
};

// Read the string representation of the operand into buf.
void OperandAsString(char *buf, Operand o) {
    switch (o.type) {
        case OPERAND_TYPE_REGISTER: {
            const char *s = RegisterAsString(o.reg);
            sprintf(buf, "%s", s);
            break;
        }
        case OPERAND_TYPE_EFFECTIVE_ADDRESS: {
            EffectiveAddressAsString(buf, o.eAddr);
            break;
        }
        case OPERAND_TYPE_IMMEDIATE: {
            sprintf(buf, "%d", o.imm);
            break;
        }
    }
}

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

Operand DecodeRegMem(u8 **p, u8 rm, u8 mod, u8 w) {
    Operand o;

    if (rm == 0x06 && mod == 0x00) {
        u8 dispLo = ReadByte(p);
        u8 dispHi = ReadByte(p);

        o.type = OPERAND_TYPE_EFFECTIVE_ADDRESS;
        o.eAddr.reg1 = REGISTER_NULL;
        o.eAddr.reg2 = REGISTER_NULL;
        o.eAddr.disp = (i16)((i16)dispLo | ((i16)dispHi << 8));
        return o;
    }

    switch (rm) {
        case 0x00:
            o.eAddr.reg1 = REGISTER_BX;
            o.eAddr.reg2 = REGISTER_SI;
            break;
        case 0x01:
            o.eAddr.reg1 = REGISTER_BX;
            o.eAddr.reg2 = REGISTER_DI;
            break;
        case 0x02:
            o.eAddr.reg1 = REGISTER_BP;
            o.eAddr.reg2 = REGISTER_SI;
            break;
        case 0x03:
            o.eAddr.reg1 = REGISTER_BP;
            o.eAddr.reg2 = REGISTER_DI;
            break;
        case 0x04:
            o.eAddr.reg1 = REGISTER_SI;
            o.eAddr.reg2 = REGISTER_NULL;
            break;
        case 0x05:
            o.eAddr.reg1 = REGISTER_DI;
            o.eAddr.reg2 = REGISTER_NULL;
            break;
        case 0x06:
            o.eAddr.reg1 = REGISTER_BP;
            o.eAddr.reg2 = REGISTER_NULL;
            break;
        case 0x07:
            o.eAddr.reg1 = REGISTER_BX;
            o.eAddr.reg2 = REGISTER_NULL;
            break;
    }

    u8 dispLo = 0;
    u8 dispHi = 0;
    switch (mod) {
        case 0x00:
            o.type = OPERAND_TYPE_EFFECTIVE_ADDRESS;
            o.eAddr.disp = 0;
            break;
        case 0x01:
            o.type = OPERAND_TYPE_EFFECTIVE_ADDRESS;
            o.eAddr.disp = ReadByte(p);
            break;
        case 0x02:
            o.type = OPERAND_TYPE_EFFECTIVE_ADDRESS;
            dispLo = *(*p)++;
            dispHi = *(*p)++;
            o.eAddr.disp = (i16)((i16)dispLo | ((i16)dispHi << 8));
            break;
        case 0x03:
            o = DecodeReg(rm, w);
            break;
    }

    return o;
}

Operand DecodeImmediate() {
    Operand o;

    // todo

    return o;
}

struct Instruction {
    Mnemonic mnemonic;
    Operand src;
    Operand dst;
};

typedef Instruction (*InstructionHandler)(u8 **p);

Instruction MovRmReg(u8 **p) {
    Instruction in;
    in.mnemonic = MNEMONIC_MOV;

    u8 b1 = ReadByte(p);
    u8 b2 = ReadByte(p);

    u8 d = (b1 >> 1) & 0x01;
    u8 w = b1 & 0x01;
    u8 mod = (b2 >> 6) & 0x03;
    u8 reg = (b2 >> 3) & 0x07;
    u8 rm = b2 & 0x07;

    Operand rmDecoded = DecodeRegMem(p, rm, mod, w);
    Operand regDecoded = DecodeReg(reg, w);

    if (d == 0) {
        in.src = regDecoded;
        in.dst = rmDecoded;
    } else {
        in.src = rmDecoded;
        in.dst = regDecoded;
    }

    return in;
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
    [0x88] = MovRmReg,
    [0x89] = MovRmReg,
    [0x8a] = MovRmReg,
    [0x8b] = MovRmReg,
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
    Instruction *ins = (Instruction *)malloc(a->size * sizeof(Instruction));

    u8 *p = a->buf;
    u8 *q = a->buf;
    size_t count = 0;
    while (p - q < a->size) {
        ins[count++] = handlerTable[*q + (p - q)](&p);
    }

    return ins;
}

void PrintInstructions(Instruction *ins) {
    char srcBuf[16];
    char dstBuf[16];

    printf("bits 16\n\n");
    for (size_t i = 0; i < sizeof(*ins); i++) {
        const char *mnemonic = MnemonicAsString(ins->mnemonic);
        OperandAsString(srcBuf, ins->src);
        OperandAsString(dstBuf, ins->dst);
        printf("%s %s, %s\n", mnemonic, dstBuf, srcBuf);
    }
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
    // printf("%s\n", a.buf);

    // todo: should be option to simulate
    Instruction *ins = DecodeInstructions(&a);
    PrintInstructions(ins);

    // // note: just an example, this only captures one simple path
    // Instruction in = ins[0];
    // const char *mnemonicStr = MnemonicAsString(in.mnemonic);
    // // todo: dst and src should not be the same thing
    // const char *srcStr = RegisterAsString(in.src.reg);
    // const char *dstStr = RegisterAsString(in.dst.reg);
    // printf("%s, %s, %s\n", mnemonicStr, dstStr, srcStr);

    free(ins);
    free(a.buf);
}
