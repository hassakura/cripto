#ifndef UTILS_H_
#define UTILS_H_

#include "structs.h"
#include <stdint.h>


void read_file(char file_name[], block_128 X[], long number_of_blocks, long file_size);

void write_to_file(char file_name[], block_128 X[], long number_of_blocks);

long get_file_size(char file_name[]);

void set_last_bits_1(block_128 * X, long file_size);

uint32_t rotate_left_32(uint32_t value_32, uint8_t value_8);

uint8_t rotate_left_8(uint8_t value_8, uint8_t value_c);

uint8_t last_5_bits(uint32_t b);

uint32_t convert_string_to_uint(char* pass_block);

void convert_32_to_8(uint32_t I, block_32 * I_t);

void switch_bytes_from_n(uint32_t * block, int start, int end);

void change_bit_in_position(int index, block_128 * b);

#endif /* UTILS_H_ */
