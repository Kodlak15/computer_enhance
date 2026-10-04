#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

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

enum Operation : u8 {
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
};

const char *OperationToString(Operation o) {
    switch (o) {
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
        case OPERATION_JLE:
            return "jle";
        case OPERATION_JNLE:
            return "jnle";
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

enum Operand : u8 {};

struct Instruction {
    Operation operation;
    Operand source;
    Operand destination;
};

Instruction *DecodeInstructions(ByteArray *a) {
    Instruction *instructions = (Instruction *)malloc(a->size * sizeof(Instruction));

    size_t offset = 0;
    size_t count = 0;
    size_t consumed;
    while (offset < a->size) {
        consumed = 0;
        // todo
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
    printf("%s\n", a.buf);

    free(a.buf);
}
