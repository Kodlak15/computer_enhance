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
    a.buf = NULL;

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
        sprintf(buf, (ea.disp > 0) ? "[%d]" : "[]", ea.disp);
    } else if (ea.reg2 == REGISTER_NULL) {
        sprintf(buf, (ea.disp > 0) ? "[%s + %d]" : "[%s]", RegisterAsString(ea.reg1), ea.disp);
    } else {
        sprintf(buf, (ea.disp > 0) ? "[%s + %s + %d]" : "[%s + %s]", RegisterAsString(ea.reg1),
                RegisterAsString(ea.reg2), ea.disp);
    }
}

enum OperandType {
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_EFFECTIVE_ADDRESS,
    OPERAND_TYPE_IMMEDIATE,
    OPERAND_TYPE_NULL,
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
        case OPERAND_TYPE_NULL: {
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
            dispLo = ReadByte(p);
            dispHi = ReadByte(p);
            o.eAddr.disp = (i16)((i16)dispLo | ((i16)dispHi << 8));
            break;
        case 0x03:
            o = DecodeReg(rm, w);
            break;
    }

    return o;
}

Operand DecodeImmediate(u8 **p, u8 s, u8 w) {
    Operand o;
    o.type = OPERAND_TYPE_IMMEDIATE;

    u8 dataLo;
    u8 dataHi;
    if (s == 0 && w == 1) {
        dataLo = ReadByte(p);
        dataHi = ReadByte(p);
        o.imm = (i16)((i16)dataLo | ((i16)dataHi << 8));
    } else {
        dataLo = ReadByte(p);
        o.imm = (i16)dataLo;
    }

    return o;
}

struct Instruction {
    Mnemonic mnemonic;
    Operand src;
    Operand dst;
};

typedef Instruction (*InstructionHandler)(u8 **p);

Instruction MovRmReg(u8 **p) {
    Instruction instruction;
    instruction.mnemonic = MNEMONIC_MOV;

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
        instruction.src = regDecoded;
        instruction.dst = rmDecoded;
    } else {
        instruction.src = rmDecoded;
        instruction.dst = regDecoded;
    }

    return instruction;
}

Instruction MovImmReg(u8 **p) {
    Instruction instruction;
    instruction.mnemonic = MNEMONIC_MOV;

    u8 b1 = ReadByte(p);

    u8 w = (b1 >> 3) & 0x01;
    u8 reg = b1 & 0x07;

    Operand immDecoded = DecodeImmediate(p, 0, w);
    Operand regDecoded = DecodeReg(reg, w);

    instruction.src = immDecoded;
    instruction.dst = regDecoded;

    return instruction;
}

Instruction AddSubCmpRmReg(u8 **p) {
    Instruction instruction;

    u8 b1 = ReadByte(p);
    u8 b2 = ReadByte(p);

    switch (b1 & 0x38) {
        case 0x00:
            instruction.mnemonic = MNEMONIC_ADD;
            break;
        case 0x28:
            instruction.mnemonic = MNEMONIC_SUB;
            break;
        case 0x38:
            instruction.mnemonic = MNEMONIC_CMP;
            break;
    }

    u8 d = (b1 >> 1) & 0x01;
    u8 w = b1 & 0x01;
    u8 mod = (b2 >> 6) & 0x03;
    u8 reg = (b2 >> 3) & 0x07;
    u8 rm = b2 & 0x07;

    Operand rmDecoded = DecodeRegMem(p, rm, mod, w);
    Operand regDecoded = DecodeReg(reg, w);

    if (d == 0) {
        instruction.src = regDecoded;
        instruction.dst = rmDecoded;
    } else {
        instruction.src = rmDecoded;
        instruction.dst = regDecoded;
    }

    return instruction;
}

Instruction AddSubCmpImmRm(u8 **p) {
    Instruction instruction;

    u8 b1 = ReadByte(p);
    u8 b2 = ReadByte(p);

    switch (b1 & 0x38) {
        case 0x00:
            instruction.mnemonic = MNEMONIC_ADD;
            break;
        case 0x28:
            instruction.mnemonic = MNEMONIC_SUB;
            break;
        case 0x38:
            instruction.mnemonic = MNEMONIC_CMP;
            break;
    }

    u8 s = (b1 >> 1) & 0x01;
    u8 w = b1 & 0x01;
    u8 mod = (b2 >> 6) & 0x03;
    u8 rm = b2 & 0x07;

    Operand rmDecoded = DecodeRegMem(p, rm, mod, w);
    Operand immDecoded = DecodeImmediate(p, s, w);

    instruction.src = immDecoded;
    instruction.dst = rmDecoded;

    return instruction;
}

Instruction AddSubCmpImmAccum(u8 **p) {
    Instruction instruction;

    u8 b1 = ReadByte(p);

    switch (b1 & 0x38) {
        case 0x00:
            instruction.mnemonic = MNEMONIC_ADD;
            break;
        case 0x28:
            instruction.mnemonic = MNEMONIC_SUB;
            break;
        case 0x38:
            instruction.mnemonic = MNEMONIC_CMP;
            break;
    }

    u8 w = b1 & 0x01;

    Operand immDecoded = DecodeImmediate(p, 0, w);
    Operand accumDecoded = DecodeReg(0x00, w);

    instruction.src = immDecoded;
    instruction.dst = accumDecoded;

    return instruction;
}

Instruction CondJmp(u8 **p) {
    Instruction instruction;

    u8 b1 = ReadByte(p);
    u8 b2 = ReadByte(p);

    switch (b1 & 0x0f) {
        case 0x00:
            instruction.mnemonic = MNEMONIC_JO;
            break;
        case 0x01:
            instruction.mnemonic = MNEMONIC_JNO;
            break;
        case 0x02:
            instruction.mnemonic = MNEMONIC_JB;
            break;
        case 0x03:
            instruction.mnemonic = MNEMONIC_JNB;
            break;
        case 0x04:
            instruction.mnemonic = MNEMONIC_JE;
            break;
        case 0x05:
            instruction.mnemonic = MNEMONIC_JNE;
            break;
        case 0x06:
            instruction.mnemonic = MNEMONIC_JL;
            break;
        case 0x07:
            instruction.mnemonic = MNEMONIC_JNL;
            break;
        case 0x08:
            instruction.mnemonic = MNEMONIC_JS;
            break;
        case 0x09:
            instruction.mnemonic = MNEMONIC_JNS;
            break;
        case 0x0a:
            instruction.mnemonic = MNEMONIC_JP;
            break;
        case 0x0b:
            instruction.mnemonic = MNEMONIC_JNP;
            break;
        case 0x0c:
            instruction.mnemonic = MNEMONIC_JL;
            break;
        case 0x0d:
            instruction.mnemonic = MNEMONIC_JNL;
            break;
        case 0x0e:
            instruction.mnemonic = MNEMONIC_JLE;
            break;
        case 0x0f:
            instruction.mnemonic = MNEMONIC_JNLE;
            break;
    }

    Operand increment;
    increment.type = OPERAND_TYPE_IMMEDIATE;
    increment.imm = b2;

    instruction.src = increment;
    instruction.dst = Operand{ .type = OPERAND_TYPE_NULL };

    return instruction;
}

Instruction Loop(u8 **p) {
    Instruction instruction;

    u8 b1 = ReadByte(p);
    u8 b2 = ReadByte(p);

    switch (b1 & 0x03) {
        case 0x00:
            instruction.mnemonic = MNEMONIC_LOOPNZ;
            break;
        case 0x01:
            instruction.mnemonic = MNEMONIC_LOOPZ;
            break;
        case 0x02:
            instruction.mnemonic = MNEMONIC_LOOP;
            break;
        case 0x03:
            instruction.mnemonic = MNEMONIC_JCXZ;
            break;
    }

    Operand increment;
    increment.type = OPERAND_TYPE_IMMEDIATE;
    increment.imm = b2;

    instruction.src = increment;
    instruction.dst = { .type = OPERAND_TYPE_NULL };

    return instruction;
}

Instruction UnhandledInstruction(u8 **p) {
    fprintf(stderr, "Error: Attempted to handle an instruction with no assigned handler.\n");
    exit(1);
}

void BuildLookupTable(InstructionHandler *table) {
    for (size_t i = 0; i < 255; i++) {
        table[i] = UnhandledInstruction;
    }

    // Add: Reg/memory with register to either
    table[0x00] = AddSubCmpRmReg;
    table[0x01] = AddSubCmpRmReg;
    table[0x02] = AddSubCmpRmReg;
    table[0x03] = AddSubCmpRmReg;
    // Add: Immediate to accumulator
    table[0x04] = AddSubCmpImmAccum;
    table[0x05] = AddSubCmpImmAccum;
    // Sub: Reg/memory and register to either
    table[0x28] = AddSubCmpRmReg;
    table[0x29] = AddSubCmpRmReg;
    table[0x2a] = AddSubCmpRmReg;
    table[0x2b] = AddSubCmpRmReg;
    // Sub: Immediate to accumulator
    table[0x2c] = AddSubCmpImmAccum;
    table[0x2d] = AddSubCmpImmAccum;
    // Cmp: Register/memory and register
    table[0x38] = AddSubCmpRmReg;
    table[0x39] = AddSubCmpRmReg;
    table[0x3a] = AddSubCmpRmReg;
    table[0x3b] = AddSubCmpRmReg;
    // Cmp: Immediate with accumulator
    table[0x3c] = AddSubCmpImmAccum;
    table[0x3d] = AddSubCmpImmAccum;
    // Add/Sub/Cmp: Immediate to/from/with register/memory (all have same first byte)
    table[0x80] = AddSubCmpImmRm;
    table[0x81] = AddSubCmpImmRm;
    table[0x82] = AddSubCmpImmRm;
    table[0x83] = AddSubCmpImmRm;
    // Move: Register/memory to/from register
    table[0x88] = MovRmReg;
    table[0x89] = MovRmReg;
    table[0x8a] = MovRmReg;
    table[0x8b] = MovRmReg;
    // Move: Immediate to register
    table[0xb0] = MovImmReg;
    table[0xb1] = MovImmReg;
    table[0xb2] = MovImmReg;
    table[0xb3] = MovImmReg;
    table[0xb4] = MovImmReg;
    table[0xb5] = MovImmReg;
    table[0xb6] = MovImmReg;
    table[0xb7] = MovImmReg;
    table[0xb8] = MovImmReg;
    table[0xb9] = MovImmReg;
    table[0xba] = MovImmReg;
    table[0xbb] = MovImmReg;
    table[0xbc] = MovImmReg;
    table[0xbd] = MovImmReg;
    table[0xbe] = MovImmReg;
    table[0xbf] = MovImmReg;
    // Jump: Conditional jump
    table[0x70] = CondJmp;
    table[0x71] = CondJmp;
    table[0x72] = CondJmp;
    table[0x73] = CondJmp;
    table[0x74] = CondJmp;
    table[0x75] = CondJmp;
    table[0x76] = CondJmp;
    table[0x77] = CondJmp;
    table[0x78] = CondJmp;
    table[0x79] = CondJmp;
    table[0x7a] = CondJmp;
    table[0x7b] = CondJmp;
    table[0x7c] = CondJmp;
    table[0x7d] = CondJmp;
    table[0x7e] = CondJmp;
    table[0x7f] = CondJmp;
    // Loop: Loop instructions
    table[0xe0] = Loop;
    table[0xe1] = Loop;
    table[0xe2] = Loop;
    table[0xe3] = Loop;
}

// Read the decoded instructions into `instructions` and return the number of instructions that were decoded.
size_t DecodeInstructions(Instruction *instructions, ByteArray *a, InstructionHandler *table) {
    u8 *p = a->buf;
    size_t count = 0;
    while (p - a->buf < a->size) {
        instructions[count++] = table[*p](&p);
    }

    return count;
}

void PrintInstructions(Instruction *instructions, size_t count) {
    char srcBuf[16];
    char dstBuf[16];

    printf("bits 16\n\n");

    for (Instruction *p = instructions; (p - instructions) < count; p++) {
        const char *mnemonic = MnemonicAsString(p->mnemonic);
        OperandAsString(srcBuf, p->src);
        OperandAsString(dstBuf, p->dst);
        if (p->dst.type == OPERAND_TYPE_NULL) {
            printf("%s %s\n", mnemonic, srcBuf);
        } else {
            printf("%s %s, %s\n", mnemonic, dstBuf, srcBuf);
        }
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

    InstructionHandler table[255];
    BuildLookupTable(table);

    Instruction *instructions = (Instruction *)malloc(a.size * sizeof(Instruction));
    size_t count = DecodeInstructions(instructions, &a, table);

    PrintInstructions(instructions, count);

    free(instructions);
    free(a.buf);
}
