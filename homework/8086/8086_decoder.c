#include "8086_decoder.h"
#include <stdlib.h>

DecoderResult decode(FileContents file) {
    DecoderResult result = {NULL, 0};

    Instruction *instructions = malloc(file.size * sizeof(*instructions));

    size_t offset = 0;
    size_t count = 0;
    while (offset < file.size) {
        size_t consumed = 0;
        Instruction instruction = decode_instruction(file, offset, &consumed);
        instructions[count++] = instruction;
        offset += consumed;
    }

    result.instructions = instructions;
    result.count = count;
    return result;
}

Instruction decode_instruction(FileContents file, size_t offset, size_t *consumed) {
    Instruction instruction;

    // todo

    return instruction;
}
