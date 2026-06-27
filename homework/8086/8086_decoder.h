#include "8086_lib.h"

typedef struct {
    Instruction *instructions;
    size_t count;
} DecoderResult;

typedef Instruction (*OperationHandler)(FileContents file, size_t offset, size_t *consumed);

DecoderResult decode(FileContents file);
Register decode_reg(uint8_t reg, uint8_t w);
EffectiveAddressBase decode_effective_address_base(uint8_t rm);
Instruction mov_rm_reg(FileContents file, size_t offset, size_t *consumed);
Instruction mov_imm_reg(FileContents file, size_t offset, size_t *consumed);
Instruction add_sub_cmp_rm_reg(FileContents file, size_t offset, size_t *consumed);
Instruction add_sub_cmp_imm_rm(FileContents file, size_t offset, size_t *consumed);
Instruction add_sub_cmp_imm_accum(FileContents file, size_t offset, size_t *consumed);
