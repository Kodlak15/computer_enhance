#include "8086_lib.h"

typedef struct {
    Instruction *instructions;
    size_t count;
} DecoderResult;

DecoderResult decode(FileContents file);
Instruction decode_instruction(FileContents file, size_t offset, size_t *consumed);
