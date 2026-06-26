#include "8086_decoder.h"
#include <stdlib.h>

Instruction mov_rm_reg(FileContents file, size_t offset, size_t consumed);
Instruction mov_imm_reg(FileContents file, size_t offset, size_t consumed);
Instruction add_rm_reg(FileContents file, size_t offset, size_t consumed);
Instruction add_imm_rm(FileContents file, size_t offset, size_t consumed);
Instruction add_imm_accum(FileContents file, size_t offset, size_t consumed);

typedef Instruction (*instruction_handler)(FileContents file, size_t offset, size_t consumed, const char *mnemonic);
instruction_handler handler_table[256];

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

    // Check this out, seems really nice: https://gist.github.com/tweetandcode/8aa6e9ce3eee0b19fd9ab0ba3c0085a3
    // User basically created a lookup table for every possible first byte that called the appropriate function

    uint8_t b1 = file.bytes[offset];
    if ((b1 & 0b11111100) == 0b10001000) {
    } else if ((b1 & 0b11111100) == 0b10000000) {
    } else if ((b1 & 0b11000100) == 0b00000100) {
    } else if ((b1 & 11110000) == 0b01110000) {
    } else if ((b1 & 0b11111100) == 0b11100000) {
    }

    return instruction;
}
