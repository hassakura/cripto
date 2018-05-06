#ifndef MAIN_H_
#define MAIN_H_

typedef uint32_t sbox [256];

typedef struct block128bits{
	uint32_t b0;
	uint32_t b1;
	uint32_t b2;
	uint32_t b3;
} block_128;

typedef struct block32bits{
	uint32_t b0;
	uint32_t b1;
	uint32_t b2;
	uint32_t b3;
} block_32;

void read_sboxes();

uint32_t rotate_left_32(uint32_t value_32, uint8_t value_8);

uint8_t rotate_left_8(uint8_t value_8, uint8_t value_c);

uint8_t last_5_bits(uint32_t b);

uint32_t convert_string_to_uint(char* pass_block);

void convert_32_to_8(uint32_t I, block_32 * I_t);

int pass_eval(char* pass);

void pass_gen (char* old_pass, block_128 * pass);

void calc_k5(int i, block_32 * k5);

void calc_k32(int i, block_128 * k32);

void calc_k(int iteration, block_128 * k, block_128 * k32, block_32 * k5);

block_32 calc_kr5(block_128 * k);

block_128 calc_km32(block_128 * k);

uint32_t calc_f1(uint32_t X, uint8_t k5, uint32_t k32);

uint32_t calc_f2(uint32_t X, uint8_t k5, uint32_t k32);

uint32_t calc_f3(uint32_t X, uint8_t k5, uint32_t k32);

block_128 UmaIteracao(int iteration, block_128 X, block_32 kr5, block_128 km32);

void tests(char* password, block_128 k, block_128 pass, block_32 k5, block_128 k32, uint32_t f1, uint32_t f2, uint32_t f3);

#endif /* MAIN_H_ */