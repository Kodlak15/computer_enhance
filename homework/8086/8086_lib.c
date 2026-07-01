#include "8086_lib.h"

#include <stdio.h>
#include <stdlib.h>

FileContents read_file(const char *path) {
    FileContents result = {NULL, 0};

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", path);
        return result;
    }

    // Seek to the end of the file
    fseek(file, 0, SEEK_END);
    // Aske where we are
    long size = ftell(file);
    // Rewind back to the start of the file
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "Couldn't find the size of file %s\n", path);
        fclose(file);
        return result;
    }

    uint8_t *buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Out of memory for file %s\n", path);
        fclose(file);
        return result;
    }

    size_t read_count = fread(buffer, 1, (size_t)size, file);
    fclose(file);

    if (read_count != (size_t)size) {
        fprintf(stderr, "Short read on %s\n", path);
        free(buffer);
        return result;
    }

    result.bytes = buffer;
    result.size = (size_t)size;
    return result;
}

const char *operation_as_str(Operation op) {
    switch (op) {
        case OPERATION_MOV:
            return "mov";
        case OPERATION_ADD:
            return "add";
        case OPERATION_SUB:
            return "sub";
        case OPERATION_CMP:
            return "cmp";
        case OPERATION_JE:
            return "je";
        case OPERATION_JL:
            return "jl";
        case OPERATION_JB:
            return "jb";
        case OPERATION_JBE:
            return "jbe";
        case OPERATION_JP:
            return "jp";
        case OPERATION_JO:
            return "jo";
        case OPERATION_JS:
            return "js";
        case OPERATION_JNE:
            return "jne";
        case OPERATION_JNL:
            return "jnl";
        case OPERATION_JG:
            return "jg";
        case OPERATION_JNB:
            return "jnb";
        case OPERATION_JA:
            return "ja";
        case OPERATION_JNP:
            return "jnp";
        case OPERATION_JNO:
            return "jno";
        case OPERATION_JNS:
            return "jns";
        case OPERATION_LOOP:
            return "loop";
        case OPERATION_LOOPZ:
            return "loopz";
        case OPERATION_LOOPNZ:
            return "loopnz";
        case OPERATION_JCXZ:
            return "jcxz";
    }
}

char *operand_as_str(Operand op, char *buf, size_t size) {
    // static char buf[32];

    switch (op.type) {
        case OPERAND_TYPE_REGISTER:
            switch (op.reg) {
                case REGISTER_AL:
                    sprintf(buf, "al");
                    break;
                case REGISTER_AX:
                    sprintf(buf, "ax");
                    break;
                case REGISTER_CL:
                    sprintf(buf, "cl");
                    break;
                case REGISTER_CX:
                    sprintf(buf, "cx");
                    break;
                case REGISTER_DL:
                    sprintf(buf, "dl");
                    break;
                case REGISTER_DX:
                    sprintf(buf, "dx");
                    break;
                case REGISTER_BL:
                    sprintf(buf, "bl");
                    break;
                case REGISTER_BX:
                    sprintf(buf, "bx");
                    break;
                case REGISTER_AH:
                    sprintf(buf, "ah");
                    break;
                case REGISTER_SP:
                    sprintf(buf, "sp");
                    break;
                case REGISTER_CH:
                    sprintf(buf, "ch");
                    break;
                case REGISTER_BP:
                    sprintf(buf, "bp");
                    break;
                case REGISTER_DH:
                    sprintf(buf, "dh");
                    break;
                case REGISTER_SI:
                    sprintf(buf, "si");
                    break;
                case REGISTER_BH:
                    sprintf(buf, "bh");
                    break;
                case REGISTER_DI:
                    sprintf(buf, "di");
                    break;
                case REGISTER_UNDEFINED:
                    // Should be unreachable
                    sprintf(buf, "");
                    break;
            };
            break;
        case OPERAND_TYPE_MEMORY:
            switch (op.effective_address.base) {
                case EA_BASE_DIRECT:
                    sprintf(buf, "[bx + si + %d]", op.effective_address.displacement);
                    break;
                case EA_BASE_BX_SI:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[bx + si + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[bx + si]");
                    }
                    break;
                case EA_BASE_BX_DI:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[bx + di + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[bx + di]");
                    }
                    break;
                case EA_BASE_BP_SI:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[bp + si + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[bp + si]");
                    }
                    break;
                case EA_BASE_BP_DI:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[bp + di + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[bp + di]");
                    }
                    break;
                case EA_BASE_SI:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[si + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[si]");
                    }
                    break;
                case EA_BASE_DI:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[di + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[di]");
                    }
                    break;
                case EA_BASE_BP:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[bp + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[bp]");
                    }
                    break;
                case EA_BASE_BX:
                    if (op.effective_address.displacement > 0) {
                        sprintf(buf, "[bx + %d]", op.effective_address.displacement);
                    } else {
                        sprintf(buf, "[bx]");
                    }
                    break;
                case EA_BASE_UNDEFINED:
                    // Should be unreachable
                    sprintf(buf, "");
                    break;
            };
            break;
        case OPERAND_TYPE_IMMEDIATE:
            sprintf(buf, "%d", op.immediate);
            break;
        case OPERAND_TYPE_NONE:
            // Should be unreachable
            sprintf(buf, "");
            break;
    }

    return buf;
}

void print_instruction(Instruction instruction) {
    Operand op1 = instruction.operands[0];
    Operand op2 = instruction.operands[1];

    char src[32];
    operand_as_str(op1, src, sizeof(src));
    char dst[32];
    operand_as_str(op2, dst, sizeof(dst));

    if (op2.type != OPERAND_TYPE_NONE) {
        printf("%s %s, %s\n", operation_as_str(instruction.operation), dst, src);
    } else {
        printf("%s %s\n", operation_as_str(instruction.operation), src);
    }
}
