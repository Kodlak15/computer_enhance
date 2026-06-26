#include "8086_decoder.h"
#include <stdlib.h>

Instruction mov_rm_reg(FileContents file, size_t offset, size_t consumed);
Instruction mov_imm_reg(FileContents file, size_t offset, size_t consumed);
Instruction add_sub_cmp_rm_reg(FileContents file, size_t offset, size_t consumed);
Instruction add_sub_cmp_imm_rm(FileContents file, size_t offset, size_t consumed);
Instruction add_sub_cmp_imm_accum(FileContents file, size_t offset, size_t consumed);

typedef Instruction (*OperationHandler)(FileContents file, size_t offset, size_t consumed);
OperationHandler handler_table[256] = {
    // Add: Reg/memory with register to either
    [0x00] = add_sub_cmp_rm_reg,
    [0x01] = add_sub_cmp_rm_reg,
    [0x02] = add_sub_cmp_rm_reg,
    [0x03] = add_sub_cmp_rm_reg,
    // Add: Immediate to accumulator
    [0x04] = add_sub_cmp_imm_accum,
    [0x05] = add_sub_cmp_imm_accum,
    // Sub: Reg/memory and register to either
    [0x28] = add_sub_cmp_rm_reg,
    [0x29] = add_sub_cmp_rm_reg,
    [0x2a] = add_sub_cmp_rm_reg,
    [0x2b] = add_sub_cmp_rm_reg,
    // Sub: Immediate to accumulator
    [0x2c] = add_sub_cmp_imm_accum,
    [0x2d] = add_sub_cmp_imm_accum,
    // Cmp: Register/memory and register
    [0x38] = add_sub_cmp_rm_reg,
    [0x39] = add_sub_cmp_rm_reg,
    [0x3a] = add_sub_cmp_rm_reg,
    [0x3b] = add_sub_cmp_rm_reg,
    // Cmp: Immediate with accumulator
    [0x3c] = add_sub_cmp_imm_accum,
    [0x3d] = add_sub_cmp_imm_accum,
    // Add/Sub/Cmp: Immediate to/from/with register/memory (all have same first byte)
    [0x80] = add_sub_cmp_imm_rm,
    [0x81] = add_sub_cmp_imm_rm,
    [0x82] = add_sub_cmp_imm_rm,
    [0x83] = add_sub_cmp_imm_rm,
    // Move: Register/memory to/from register
    [0x88] = mov_rm_reg,
    [0x89] = mov_rm_reg,
    [0x8a] = mov_rm_reg,
    [0x8b] = mov_rm_reg,
    // Move: Immediate to register
    [0xb0] = mov_imm_reg,
    [0xb1] = mov_imm_reg,
    [0xb2] = mov_imm_reg,
    [0xb3] = mov_imm_reg,
    [0xb4] = mov_imm_reg,
    [0xb5] = mov_imm_reg,
    [0xb6] = mov_imm_reg,
    [0xb7] = mov_imm_reg,
    [0xb8] = mov_imm_reg,
    [0xb9] = mov_imm_reg,
    [0xba] = mov_imm_reg,
    [0xbb] = mov_imm_reg,
    [0xbc] = mov_imm_reg,
    [0xbd] = mov_imm_reg,
    [0xbe] = mov_imm_reg,
    [0xbf] = mov_imm_reg,
};

DecoderResult decode(FileContents file) {
    DecoderResult result = {NULL, 0};

    // Maximum number of bytes that need to be allocated is the total number of bytes that are read from the file
    // multiplied by the size of `Instruction`
    Instruction *instructions = malloc(file.size * sizeof(*instructions));

    size_t offset = 0;
    size_t count = 0;
    while (offset < file.size) {
        size_t consumed = 0;
        // Instruction instruction = decode_instruction(file, offset, &consumed);
        Instruction instruction = handler_table[file.bytes[offset]](file, offset, consumed);
        instructions[count++] = instruction;
        offset += consumed;
    }

    result.instructions = instructions;
    result.count = count;
    return result;
}

// Instruction decode_instruction(FileContents file, size_t offset, size_t *consumed) {
//     Instruction instruction;
//
//     // Check this out, seems really nice: https://gist.github.com/tweetandcode/8aa6e9ce3eee0b19fd9ab0ba3c0085a3
//     // User basically created a lookup table for every possible first byte that called the appropriate function
//
//     uint8_t b1 = file.bytes[offset];
//     if ((b1 & 0b11111100) == 0b10001000) {
//     } else if ((b1 & 0b11111100) == 0b10000000) {
//     } else if ((b1 & 0b11000100) == 0b00000100) {
//     } else if ((b1 & 11110000) == 0b01110000) {
//     } else if ((b1 & 0b11111100) == 0b11100000) {
//     }
//
//     return instruction;
// }
