#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "utils.h"


/*	Recebe o nome do arquivo a ser lido, guarda o conteudo em X e caso o
	comprimento do arquivo nao seja multiplo de 16 bytes, completa o
	ultimo bloco com bits no valor 1
*/

void read_file(char file_name[], block_128 X[], uint32_t number_of_blocks, uint32_t file_size) {
    FILE *p_input_file;
    int i;
    p_input_file = fopen(file_name, "rb");

    if (p_input_file == NULL) {
        printf("Input file %s not found.\n", file_name);
        exit(1);
    }
    for (i = 0; i < number_of_blocks - 1; i++){
    	fread(&X[i], 1, 16, p_input_file);
		if (i == number_of_blocks - 2) set_last_bits_1(&X[i], file_size);
	}
	if (file_size % 16 > 0) set_last_bits_1(&X[number_of_blocks - 1], 4);
    fclose(p_input_file);
}

/*	Recebe o nome do arquivo a ser escrito, e cria esse arquivo com
	o conteudo de X
*/

void write_to_file(char file_name[], block_128 X[], uint32_t file_size) {
    FILE *p_output_file;
    int i;
    p_output_file = fopen(file_name, "wb");
    if (p_output_file == NULL) {
        printf("Output file %s not found.\n", file_name);
        exit(1);
    }
    fwrite(X, 1, file_size, p_output_file);
    fclose(p_output_file);
}

uint32_t get_file_size(char file_name[]) {
    FILE *p_input_file;
    long file_size;

    p_input_file = fopen(file_name, "rb");
    if (p_input_file == NULL) {
        printf("Input file %s not found.\n", file_name);
        exit(1);
    }

    fseek(p_input_file, 0, SEEK_END);
    file_size = ftell(p_input_file);
    fseek(p_input_file, 0, SEEK_SET);
    fclose(p_input_file);

    return file_size;
}

/*	Recebe o tamanho do arquivo e completa o conteudo dele
	com bits 1, ate que file_size % 16 == 0
*/

void set_last_bits_1(block_128 * X, uint32_t file_size){
	long bytes_to_switch;
	bytes_to_switch = file_size % 16;
	if (bytes_to_switch > 12)
		switch_bytes_from_n(&X->b3, bytes_to_switch, 16);
	else if (bytes_to_switch > 8){
		X->b3 |= 0xFFFFFFFF;
		switch_bytes_from_n(&X->b2, bytes_to_switch, 12);
	}
	else if (bytes_to_switch > 4){
		X->b3 |= 0xFFFFFFFF;
		X->b2 |= 0xFFFFFFFF;
		switch_bytes_from_n(&X->b1, bytes_to_switch, 8);
	}
	else if (bytes_to_switch == 0);
	else{
		X->b3 |= 0xFFFFFFFF;
		X->b2 |= 0xFFFFFFFF;
		X->b1 |= 0xFFFFFFFF;
		switch_bytes_from_n(&X->b0, bytes_to_switch, 4);
	}
}

/*	Rotacao circular para esquerda com bloco de 32 bits */

uint32_t rotate_left_32(uint32_t value_32, uint8_t value_8){
    return (value_32<<value_8) | (value_32>>(32-value_8));
}

/*	Rotacao circular para esquerda com bloco de 8 bits */

uint8_t rotate_left_8(uint8_t value_8, uint8_t value_c){
    return (value_8<<value_c) | (value_8>>(8-value_c));
}

/*	Recebe um bloco de 32 bits e retorna o valor dos 5 ultimos
	bits
*/

uint8_t last_5_bits(uint32_t b){
	return b & 0x1f;
}

/* 	Recebe uma string pass_block e retorna sua 
	representacao hexadecimal
*/

uint32_t convert_string_to_uint(char* pass_block){
	uint32_t hex_rep;
	int i;
	for (i=0; i<4; ++i)
    	hex_rep = (hex_rep << 8) | pass_block[i];
    return hex_rep;
}

/*	Recebe um bloco I de 32 bits, e copia-o para um
	block_32 (4 x 8 bits)
*/

void convert_32_to_8(uint32_t I, block_32 * I_t){
	I_t->b0 = (uint8_t)(I >> 24);
	I_t->b1 = (uint8_t)(I >> 16);
	I_t->b2 = (uint8_t)(I >> 8);
	I_t->b3 = (uint8_t)(I);
}

/*	Recebe um bloco de 32 bits, e troca o valor dos bits de
	start ate end
*/

void switch_bytes_from_n(uint32_t * block, int start, int end){
	int i;
	for (i = start; i < end; i++)
		* block |= (0xFF << (8 * i));
}

/*	Recebe a posicao do bit a ser trocado, e inverte seu
	valor (index vai de 0..file_size)
*/

void change_bit_in_position(int index, block_128 * b){
	int block_position = index % 128;
	if (block_position > 96)
		b->b3 ^= 1LL << (block_position - 96);
	else if (block_position > 64)
		b->b2 ^= 1LL << (block_position - 64);
	else if (block_position > 32)
		b->b1 ^= 1LL << (block_position - 32);
	else
		b->b0 ^= 1LL << block_position;
}

