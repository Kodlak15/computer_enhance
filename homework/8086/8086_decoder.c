#include "8086_decoder.h"
#include <stdint.h>
#include <stdlib.h>

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
        Instruction instruction = handler_table[file.bytes[offset]](file, offset, &consumed);
        instructions[count++] = instruction;
        offset += consumed;
    }

    result.instructions = instructions;
    result.count = count;
    return result;
}

Register decode_reg(uint8_t reg, uint8_t w) {
    switch (reg) {
        case 0x00: {
            return w == 0 ? REGISTER_AL : REGISTER_AH;
        };
        case 0x01: {
            return w == 0 ? REGISTER_CL : REGISTER_CX;
        };
        case 0x02: {
            return w == 0 ? REGISTER_DL : REGISTER_DX;
        };
        case 0x03: {
            return w == 0 ? REGISTER_BL : REGISTER_BX;
        };
        case 0x04: {
            return w == 0 ? REGISTER_AH : REGISTER_SP;
        };
        case 0x05: {
            return w == 0 ? REGISTER_CH : REGISTER_BP;
        };
        case 0x06: {
            return w == 0 ? REGISTER_DH : REGISTER_SI;
        };
        case 0x07: {
            return w == 0 ? REGISTER_BH : REGISTER_DI;
        };
        default: {
            return REGISTER_UNDEFINED;
        };
    }
}

EffectiveAddressBase decode_effective_address_base(uint8_t rm) {
    switch (rm) {
        case 0x00: {
            return EA_BASE_BX_SI;
        };
        case 0x01: {
            return EA_BASE_BX_DI;
        };
        case 0x02: {
            return EA_BASE_BP_SI;
        };
        case 0x03: {
            return EA_BASE_BP_DI;
        };
        case 0x04: {
            return EA_BASE_SI;
        };
        case 0x05: {
            return EA_BASE_DI;
        };
        case 0x06: {
            return EA_BASE_BP;
        };
        case 0x07: {
            return EA_BASE_BX;
        };
        default: {
            return EA_BASE_UNDEFINED;
        };
    }
}

Instruction mov_rm_reg(FileContents file, size_t offset, size_t *consumed) {
    Instruction result = {};

    uint8_t b1 = file.bytes[offset++];
    ++consumed;
    uint8_t b2 = file.bytes[offset++];
    ++consumed;

    uint8_t d = (b1 >> 1) & 0x01;
    uint8_t w = b1 & 0x01;
    uint8_t mod = (b2 >> 6) & 0x03;
    uint8_t reg = (b2 >> 3) & 0x04;
    uint8_t rm = b2 & 0x04;

    Operand rm_decoded;
    switch (mod) {
        case 0x00: {
            if (rm == 0x03) {
                uint8_t disp_lo = file.bytes[offset++];
                ++consumed;
                uint8_t disp_hi = file.bytes[offset++];
                ++consumed;
                int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));

                rm_decoded.type = OPERAND_TYPE_MEMORY;
                rm_decoded.effective_address.base = EA_BASE_DIRECT;
                rm_decoded.effective_address.displacement = disp;
            } else {
            }
        } break;
        case 0x01: {
        } break;
        case 0x10: {
        } break;
        case 0x11: {
        } break;
    }

    Operand reg_decoded = {
        OPERAND_TYPE_REGISTER,
        decode_reg(reg, w),
    };

    return result;
}

Instruction mov_imm_reg(FileContents file, size_t offset, size_t *consumed) {
    Instruction result = {};

    return result;
}

Instruction add_sub_cmp_rm_reg(FileContents file, size_t offset, size_t *consumed) {
    Instruction result = {};

    return result;
}

Instruction add_sub_cmp_imm_rm(FileContents file, size_t offset, size_t *consumed) {
    Instruction result = {};

    return result;
}

Instruction add_sub_cmp_imm_accum(FileContents file, size_t offset, size_t *consumed) {
    Instruction result = {};

    return result;
}
