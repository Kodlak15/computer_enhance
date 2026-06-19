#include <stdint.h>
#include <stdio.h>

void decode_file(FILE *fptr);
const char *decode_reg(unsigned char reg, int is_word);
void decode_effective_address(char buf[], char rm, int16_t disp);
void decode_rm_reg(FILE *fptr, int b1, const char *mnemonic);
void decode_imm_reg(FILE *fptr, int b1, const char *mnemonic);
void decode_imm_rm(FILE *fptr, int b1, int b2, const char *mnemonic);
void decode_imm_accum(FILE *fptr, int b1, const char *mnemonic);
void decode_cond_jmp(FILE *fptr, const char *mnemonic);
void decode_loop(FILE *fptr, const char *mnemonic);
