#include "8086_decoder.h"
#include <stdlib.h>

DecoderResult decode(FileContents file) {
    DecoderResult result = {NULL, 0};

    // Maximum number of bytes that need to be allocated is the total number of bytes that are read from the file
    // multiplied by the size of `Instruction`
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

    // TODO: Check opcodes and do all of the stuff I was doing before right here

    return instruction;
}
