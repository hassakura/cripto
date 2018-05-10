#ifndef K128_H_
#define K128_H_

#include "structs.h"
#include <inttypes.h>
#include <ctype.h>
#include <limits.h>


void read_sboxes();

void set_k0(block_128 * k, block_128 * pass);

int pass_eval(char* pass);

void pass_gen (char* old_pass, block_128 * pass, block_128 * k);

block_32 calc_k5(int i);

block_128 calc_k32(int i);

void calc_k(int iteration, block_128 * k);

block_32 calc_kr5(block_128 * k);

block_128 calc_km32(block_128 * k);

uint32_t calc_f1(uint32_t X, uint8_t k5, uint32_t k32);

uint32_t calc_f2(uint32_t X, uint8_t k5, uint32_t k32);

uint32_t calc_f3(uint32_t X, uint8_t k5, uint32_t k32);

block_128 UmaIteracao(block_128 X, block_32 kr5, block_128 km32);

block_128 UmaIteracao_inv(block_128 X, block_32 kr5, block_128 km32);

void encrypt_k128(block_128 * X, block_128 * k, block_128 * pass, long number_of_blocks);

void decrypt_k128(block_128 * X, block_128 * all_subkeys, block_128 * k, block_128 * pass, long number_of_blocks);

int hamming_dist(block_128 BlC, block_128 BlAC);

void hamming_K128(block_128 * X, block_128 * all_subkeys, block_128 * k, block_128 * pass, long number_of_blocks, int flag);

#endif /* K128_H_ */
