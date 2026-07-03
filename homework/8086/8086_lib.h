#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t *bytes;
    size_t size;
} FileContents;

typedef enum {
    OPERATION_MOV,
    OPERATION_ADD,
    OPERATION_SUB,
    OPERATION_CMP,
    OPERATION_JE,
    OPERATION_JL,
    OPERATION_JB,
    OPERATION_JBE,
    OPERATION_JP,
    OPERATION_JO,
    OPERATION_JS,
    OPERATION_JNE,
    OPERATION_JNL,
    OPERATION_JG,
    OPERATION_JNB,
    OPERATION_JA,
    OPERATION_JNP,
    OPERATION_JNO,
    OPERATION_JNS,
    OPERATION_JLE,
    OPERATION_JNLE,
    OPERATION_LOOP,
    OPERATION_LOOPZ,
    OPERATION_LOOPNZ,
    OPERATION_JCXZ,
} Operation;

typedef enum {
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
    REGISTER_UNDEFINED,
} Register;

typedef enum {
    EA_BASE_DIRECT,
    EA_BASE_BX_SI,
    EA_BASE_BX_DI,
    EA_BASE_BP_SI,
    EA_BASE_BP_DI,
    EA_BASE_SI,
    EA_BASE_DI,
    EA_BASE_BP,
    EA_BASE_BX,
    EA_BASE_UNDEFINED,
} EffectiveAddressBase;

typedef struct {
    EffectiveAddressBase base;
    int16_t displacement;
} EffectiveAddress;

typedef enum {
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_MEMORY,
    OPERAND_TYPE_IMMEDIATE,
    OPERAND_TYPE_NONE,
} OperandType;

typedef struct {
    OperandType type;
    union {
        Register reg;
        EffectiveAddress effective_address;
        int16_t immediate;
    };
} Operand;

typedef struct {
    Operation operation;
    Operand operands[2];
} Instruction;

FileContents read_file(const char *path);
const char *operation_as_str(Operation op);
char *operand_as_str(Operand op, char *buf, size_t size);
void print_instruction(Instruction instruction);
