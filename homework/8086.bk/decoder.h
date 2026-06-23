#include "helpers.h"
#include <stdint.h>
#include <stdio.h>

void decode_8086(FileContents file);
const char *decode_reg(unsigned char reg, int is_word);
void decode_effective_address(char buf[], char rm, int16_t disp);
void decode_rm_reg(FileContents file, int b1, const char *mnemonic);
void decode_imm_reg(FileContents file, int b1, const char *mnemonic);
void decode_imm_rm(FileContents file, int b1, int b2, const char *mnemonic);
void decode_imm_accum(FileContents file, int b1, const char *mnemonic);
void decode_cond_jmp(FileContents file, const char *mnemonic);
void decode_loop(FileContents file, const char *mnemonic);
