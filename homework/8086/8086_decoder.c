#include "8086_decoder.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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
        Instruction instruction = handler_table[file.bytes[offset]](file, offset, &consumed);
        instructions[count++] = instruction;
        offset += consumed;
    }

    result.instructions = instructions;
    result.count = count;
    return result;
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

Operand decode_immediate(FileContents file, size_t offset, size_t *consumed, uint8_t w) {
    Operand result;
    result.type = OPERAND_TYPE_IMMEDIATE;

    uint8_t data_lo = file.bytes[offset++];
    *consumed += 1;

    if (w == 1) {
        uint8_t data_hi = file.bytes[offset++];
        *consumed += 1;
        result.immediate = (int16_t)((int16_t)data_lo | ((int16_t)data_hi << 8));
    } else {
        result.immediate = (int8_t)data_lo;
    }

    return result;
}

Operand decode_rm(FileContents file, size_t offset, size_t *consumed, uint8_t rm, uint8_t mod, uint8_t w) {
    Operand result;

    switch (mod) {
        case 0x00: {
            if (rm == 0x06) {
                uint8_t disp_lo = file.bytes[offset++];
                *consumed += 1;
                uint8_t disp_hi = file.bytes[offset++];
                *consumed += 1;
                int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));

                result.type = OPERAND_TYPE_MEMORY;
                result.effective_address.base = EA_BASE_DIRECT;
                result.effective_address.displacement = disp;
            } else {
                result.type = OPERAND_TYPE_MEMORY;
                result.effective_address.base = decode_effective_address_base(rm);
                result.effective_address.displacement = 0;
            }
            break;
        }
        case 0x01: {
            int16_t disp = (int8_t)file.bytes[offset++];
            *consumed += 1;

            result.type = OPERAND_TYPE_MEMORY;
            result.effective_address.base = decode_effective_address_base(rm);
            result.effective_address.displacement = disp;
            break;
        }
        case 0x02: {
            uint8_t disp_lo = file.bytes[offset++];
            *consumed += 1;
            uint8_t disp_hi = file.bytes[offset++];
            *consumed += 1;
            int16_t disp = (int16_t)((uint16_t)disp_lo | ((uint16_t)disp_hi << 8));

            result.type = OPERAND_TYPE_MEMORY;
            result.effective_address.base = decode_effective_address_base(rm);
            result.effective_address.displacement = disp;
        } break;
        case 0x03: {
            result = decode_reg(rm, w);
        } break;
    }

    return result;
}

Operand decode_reg(uint8_t reg, uint8_t w) {
    Operand result;
    result.type = OPERAND_TYPE_REGISTER;

    switch (reg) {
        case 0x00:
            result.reg = w == 0 ? REGISTER_AL : REGISTER_AX;
            break;
        case 0x01: {
            result.reg = w == 0 ? REGISTER_CL : REGISTER_CX;
            break;
        };
        case 0x02: {
            result.reg = w == 0 ? REGISTER_DL : REGISTER_DX;
            break;
        };
        case 0x03: {
            result.reg = w == 0 ? REGISTER_BL : REGISTER_BX;
            break;
        };
        case 0x04: {
            result.reg = w == 0 ? REGISTER_AH : REGISTER_SP;
            break;
        };
        case 0x05: {
            result.reg = w == 0 ? REGISTER_CH : REGISTER_BP;
            break;
        };
        case 0x06: {
            result.reg = w == 0 ? REGISTER_DH : REGISTER_SI;
            break;
        };
        case 0x07: {
            result.reg = w == 0 ? REGISTER_BH : REGISTER_DI;
            break;
        };
        default: {
            result.reg = REGISTER_UNDEFINED;
            break;
        };
    }

    return result;
}

Instruction mov_rm_reg(FileContents file, size_t offset, size_t *consumed) {
    Instruction instruction;
    instruction.operation = OPERATION_MOV;

    uint8_t b1 = file.bytes[offset++];
    *consumed += 1;
    uint8_t b2 = file.bytes[offset++];
    *consumed += 1;

    uint8_t d = (b1 >> 1) & 0x01;
    uint8_t w = b1 & 0x01;
    uint8_t mod = (b2 >> 6) & 0x03;
    uint8_t reg = (b2 >> 3) & 0x07;
    uint8_t rm = b2 & 0x07;

    Operand rm_decoded = decode_rm(file, offset, consumed, rm, mod, w);
    Operand reg_decoded = decode_reg(reg, w);

    if (d == 0) {
        instruction.operands[0] = reg_decoded;
        instruction.operands[1] = rm_decoded;
    } else {
        instruction.operands[0] = rm_decoded;
        instruction.operands[1] = reg_decoded;
    }

    return instruction;
}

Instruction mov_imm_reg(FileContents file, size_t offset, size_t *consumed) {
    Instruction instruction;
    instruction.operation = OPERATION_MOV;

    uint8_t b1 = file.bytes[offset++];
    *consumed += 1;

    uint8_t w = (b1 >> 3) & 0x01;
    uint8_t reg = b1 & 0x07;

    Operand imm_decoded = decode_immediate(file, offset, consumed, w);
    Operand reg_decoded = decode_reg(reg, w);

    instruction.operands[0] = imm_decoded;
    instruction.operands[1] = reg_decoded;

    return instruction;
}

Instruction add_sub_cmp_rm_reg(FileContents file, size_t offset, size_t *consumed) {
    Instruction instruction;

    uint8_t b1 = file.bytes[offset++];
    *consumed += 1;
    uint8_t b2 = file.bytes[offset++];
    *consumed += 1;

    switch (b1 & 0x38) {
        case 0x00:
            instruction.operation = OPERATION_ADD;
            break;
        case 0x28:
            instruction.operation = OPERATION_SUB;
            break;
        case 0x38:
            instruction.operation = OPERATION_CMP;
            break;
    }

    uint8_t d = (b1 >> 1) & 0x01;
    uint8_t w = b1 & 0x01;
    uint8_t mod = (b2 >> 6) & 0x03;
    uint8_t reg = (b2 >> 3) & 0x07;
    uint8_t rm = b2 & 0x07;

    Operand rm_decoded = decode_rm(file, offset, consumed, rm, mod, w);
    Operand reg_decoded = decode_reg(reg, w);

    if (d == 0) {
        instruction.operands[0] = reg_decoded;
        instruction.operands[1] = rm_decoded;
    } else {
        instruction.operands[0] = rm_decoded;
        instruction.operands[1] = reg_decoded;
    }

    return instruction;
}

Instruction add_sub_cmp_imm_rm(FileContents file, size_t offset, size_t *consumed) {
    Instruction instruction;

    uint8_t b1 = file.bytes[offset++];
    *consumed += 1;
    uint8_t b2 = file.bytes[offset++];
    *consumed += 1;

    uint8_t s = (b1 >> 1) & 0x01;
    uint8_t w = b1 & 0x01;
    uint8_t mod = (b2 >> 6) & 0x03;
    uint8_t rm = b2 & 0x07;

    size_t consumed_before = *consumed;
    Operand rm_decoded = decode_rm(file, offset, consumed, rm, mod, w);
    offset += *consumed - consumed_before; // update offset to reflect bytes read in decode_rm
    Operand imm_decoded = decode_immediate(file, offset, consumed, w);

    // TODO handle converting this part
    // const char *dst = rm_buf;
    // if (mod == 0b11) {
    //     printf("%s %s, %d\n", mnemonic, dst, data);
    // } else if (w == 1) {
    //     printf("%s word %s, %d\n", mnemonic, dst, data);
    // } else {
    //     printf("%s byte %s, %d\n", mnemonic, dst, data);
    // }

    return instruction;
}

Instruction add_sub_cmp_imm_accum(FileContents file, size_t offset, size_t *consumed) {
    Instruction instruction;

    uint8_t b1 = file.bytes[offset++];
    *consumed += 1;

    uint8_t w = b1 & 0x01;

    size_t consumed_before = *consumed;
    Operand imm_decoded = decode_immediate(file, offset, consumed, w);

    Operand accum_decoded;
    accum_decoded.type = OPERAND_TYPE_REGISTER;
    if (w == 0) {
        accum_decoded.reg = REGISTER_AX;
    } else {
        accum_decoded.reg = REGISTER_AL;
    }

    instruction.operands[0] = imm_decoded;
    instruction.operands[1] = accum_decoded;

    return instruction;
}

// TODO handle jump/loop instructions
