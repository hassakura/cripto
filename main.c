#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include "main.h"

/* 	uint8_t = 	1 byte 	= 8  bits
	uint32_t = 	4 bytes	= 32 bits
	uint64_t = 	8 bytes = 64 bits
						*/



#define MOD32 4294967296


/*	GLOBAL VARS	*/

sbox s1, s2, s3, s4;
uint8_t ConstR = 0b0010011;
uint32_t ConstM = 0xCB3725F7;

/*	UTIL FUNCS	*/

void read_sboxes(){
	FILE* f_sbox_1;
	FILE* f_sbox_2;
	FILE* f_sbox_3;
	FILE* f_sbox_4;
	int i = 0;
	f_sbox_1 = fopen("sbox1", "r");
	f_sbox_2 = fopen("sbox2", "r"); 
	f_sbox_3 = fopen("sbox3", "r");
	f_sbox_4 = fopen("sbox4", "r");
	if (f_sbox_1 && f_sbox_2 && f_sbox_3 && f_sbox_4){
		while(!feof(f_sbox_4)){
			fscanf(f_sbox_1, "%x", &s1[i]);
			fscanf(f_sbox_2, "%x", &s2[i]);
			fscanf(f_sbox_3, "%x", &s3[i]);
			fscanf(f_sbox_4, "%x", &s4[i++]);
		}
		fclose(f_sbox_1);
		fclose(f_sbox_2);
		fclose(f_sbox_3);
		fclose(f_sbox_4);
	}
	else printf("Cant read sbox files");
}

uint32_t rotate_left_32(uint32_t value_32, uint8_t value_8){
    return (value_32<<value_8) | (value_32>>(32-value_8));
}

uint8_t rotate_left_8(uint8_t value_8, uint8_t value_c){
    return (value_8<<value_c) | (value_8>>(8-value_c));
}

uint8_t last_5_bits(uint32_t b){
	return b & 0x1f;
}

uint32_t convert_string_to_uint(char* pass_block){
	uint32_t hex_rep;
	int i;
	for (i=0; i<4; ++i)
    	hex_rep = (hex_rep << 8) | pass_block[i];
    return hex_rep;
}

void convert_32_to_8(uint32_t I, block_32 * I_t){
	I_t->b0 = (uint8_t)(I >> 24);
	I_t->b1 = (uint8_t)(I >> 16);
	I_t->b2 = (uint8_t)(I >> 8);
	I_t->b3 = (uint8_t)(I);
}

void switch_bytes_from_n(uint32_t * block, int start, int end){
	int i;
	for (i = start; i < end; i++)
		* block |= (0xFF << (8 * i));
}

void set_last_bits_1(block_128 * X, long file_size){
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

/* -------------------------------------------- */

/* 	FILE HANDLER	*/

void read_file(char file_name[], block_128 X[], long number_of_blocks, long file_size) {
    FILE *p_input_file;
    int i;
    p_input_file = fopen(file_name, "rb");

    if (p_input_file == NULL) {
        printf("Input file %s not found.\n", file_name);
        exit(1);
    }
    for (i = 0; i < number_of_blocks; i++){
    	fread(&X[i], 1, 16, p_input_file);
		if (i == number_of_blocks - 1) set_last_bits_1(&X[i], file_size);
	}
    fclose(p_input_file);
}

void write_to_file(char file_name[], block_128 X[], long number_of_blocks) {
    FILE *p_output_file;
    int i;
    p_output_file = fopen(file_name, "wb");
    if (p_output_file == NULL) {
        printf("Output file %s not found.\n", file_name);
        exit(1);
    }
    for (i = 0; i < number_of_blocks; i++){
    	fwrite(&X[i], 1, 16, p_output_file);
    }
    fclose(p_output_file);
}

long get_file_size(char file_name[]) {
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



/*	PASSWORD CONFIGS	*/

int pass_eval(char* pass){
	int letters = 0, numbers = 0;
	int i;
	for (i = 0; pass[i] != '\0'; i++){
		if ((pass[i] >= 'a' && pass[i] <= 'z') || (pass[i] >= 'A' && pass[i] <= 'Z')) letters += 1;
		else if ((pass[i] >= '0' && pass[i] <= '9')) numbers += 1;
		else;
	}
	return (numbers > 1 && letters > 1 && i >= 8);
}

void pass_gen (char* old_pass, block_128 * pass, block_128 * k){
	size_t pass_len = strlen(old_pass);
	char* new_pass = malloc(16 * sizeof (uint8_t));
	
	
	if (pass_len < 16){
		memcpy(new_pass, old_pass, pass_len);
		memcpy(new_pass + pass_len, old_pass, 16 - pass_len);
	} 
	else memcpy(new_pass, old_pass, 16);
	/*printf("new_pass: %s\n", new_pass);*/
	
	pass->b0 = convert_string_to_uint (new_pass);
	pass->b1 = convert_string_to_uint (new_pass + 4);
	pass->b2 = convert_string_to_uint (new_pass + 8);
	pass->b3 = convert_string_to_uint (new_pass + 12);

	set_k0(k, pass);

}

/*	INTERM KEYS	*/

block_32 calc_k5(int i){ 
	
	block_32 k5;

	k5.b0 = rotate_left_8(ConstR, ((int) pow(i + 2, 2) % 3));
	k5.b1 = rotate_left_8(ConstR, ((int) pow(i + 2, 1) % 3));
	k5.b2 = rotate_left_8(ConstR, ((int) pow(i + 2, 3) % 3));
	k5.b3 = rotate_left_8(ConstR, ((int) pow(i + 2, 2) % 3));
	
	return k5;

}


block_128 calc_k32(int i){
	
	block_128 k32;

	k32.b0 = rotate_left_32(ConstM, ((int) pow(i + 3, 2) % 7));
	k32.b1 = rotate_left_32(ConstM, ((int) pow(i + 3, 1) % 7));
	k32.b2 = rotate_left_32(ConstM, ((int) pow(i + 3, 3) % 7));
	k32.b3 = rotate_left_32(ConstM, ((int) pow(i + 3, 2) % 7));
	
	return k32;

}

void calc_k(int iteration, block_128 * k){

	block_32 k5;
	block_128 k32;

	k5 = calc_k5(iteration);
	k32 = calc_k32(iteration);
	k->b0 = k->b0 ^ calc_f2(k->b3, k5.b0, k32.b0);
	k->b1 = k->b1 ^ calc_f1(k->b0, k5.b1, k32.b1);
	k->b2 = k->b2 ^ calc_f3(k->b1, k5.b2, k32.b2);
	k->b3 = k->b3 ^ calc_f2(k->b2, k5.b3, k32.b3);
}


/*	SUBKEYS KR5 & KM32 	*/


block_32 calc_kr5(block_128 * k){

	block_32 kr5;
	kr5.b0 = last_5_bits(k->b3);
	kr5.b1 = last_5_bits(k->b2);
	kr5.b2 = last_5_bits(k->b1);
	kr5.b3 = last_5_bits(k->b0);
	return kr5;
}

block_128 calc_km32(block_128 * k){
	block_128 km32;
	km32.b0 = k->b0;
	km32.b1 = k->b1;
	km32.b2 = k->b2;
	km32.b3 = k->b3;
	return km32;
}


/*	FUNCTIONS (f1, f2, f3)	*/

uint32_t calc_f1(uint32_t X, uint8_t k5, uint32_t k32){
	uint32_t I, Y;
	block_32 I_t;

	I = rotate_left_32((k32 ^ X) % MOD32, k5);
	convert_32_to_8(I, &I_t);
	Y = (((s1[I_t.b0] + s2[I_t.b1]) - s3[I_t.b2]) ^ s4[I_t.b3]) % MOD32;
	return Y;
}

uint32_t calc_f2(uint32_t X, uint8_t k5, uint32_t k32){
	uint32_t I, Y;
	block_32 I_t;

	I = rotate_left_32((k32 ^ X) % MOD32, k5);
	convert_32_to_8(I, &I_t);
	Y = (((s1[I_t.b0] - s2[I_t.b1]) ^ s3[I_t.b2]) + s4[I_t.b3]) % MOD32;
	return Y;
}

uint32_t calc_f3(uint32_t X, uint8_t k5, uint32_t k32){
	uint32_t I, Y;
	block_32 I_t;

	I = rotate_left_32((k32 ^ X) % MOD32, k5);
	convert_32_to_8(I, &I_t);
	Y = (((s1[I_t.b0] ^ s2[I_t.b1]) + s3[I_t.b2]) - s4[I_t.b3]) % MOD32;
	return Y;
}

/*	MAIN ENCRYPT	*/


block_128 UmaIteracao(block_128 X, block_32 kr5, block_128 km32){
	

	//printf("C: %x ^ %x\n", X.b2, calc_f2(X.b3, kr5.b0, km32.b0));
	X.b2 = X.b2 ^ calc_f2(X.b3, kr5.b0, km32.b0);
	//printf("B: %x ^ %x\n", X.b1, calc_f2(X.b2, kr5.b1, km32.b1));
	X.b1 = X.b1 ^ calc_f1(X.b2, kr5.b1, km32.b1);
	//printf("A: %x ^ %x\n", X.b0, calc_f2(X.b1, kr5.b2, km32.b2));
	X.b0 = X.b0 ^ calc_f3(X.b1, kr5.b2, km32.b2);
	//printf("D: %x ^ %x\n", X.b3, calc_f2(X.b0, kr5.b3, km32.b3));
	X.b3 = X.b3 ^ calc_f2(X.b0, kr5.b3, km32.b3);


	return X;
}

block_128 UmaIteracao_inv(block_128 X, block_32 kr5, block_128 km32){
	
	block_128 old_block;

	old_block.b1 = X.b1 ^ calc_f1(X.b2, kr5.b1, km32.b1);
	old_block.b0 = X.b0 ^ calc_f3(X.b1, kr5.b2, km32.b2);
	old_block.b3 = X.b3 ^ calc_f2(X.b0, kr5.b3, km32.b3);
	old_block.b2 = X.b2 ^ calc_f2(old_block.b3, kr5.b0, km32.b0);

	return old_block;
}

void calc_all_ks(block_128 * all_subkeys, block_128 * k){
	int i;
	for (i = 0; i < 12; i++){
		calc_k(i, k);
		memcpy (&all_subkeys[i], k, sizeof(block_128));
	}
}

void set_k0(block_128 * k, block_128 * pass){
	k->b0 = 0x5A827999 ^ pass->b0;
	k->b1 = 0x874AA67D ^ pass->b1;
	k->b2 = 0x657B7C8E ^ pass->b2;
	k->b3 = 0xBD070242 ^ pass->b3;
}

void encrypt_k128(block_128 * X, block_128 * k, block_128 * pass, long number_of_blocks){
	int i, j;
	for (j = 0; j < number_of_blocks; j++){
		/*printf("oe: %x %x %x %x\n", X[j].b0, X[j].b1, X[j].b2, X[j].b3);*/
		printf("X[%d] B: %x %x %x %x\n",j, X[j].b0, X[j].b1, X[j].b2, X[j].b3);
		for (i = 0; i < 12; i++){
			calc_k(i, k);
		    X[j] = UmaIteracao(X[j], calc_kr5(k), calc_km32(k));
		}
		printf("X[%d] A: %x %x %x %x\n",j, X[j].b0, X[j].b1, X[j].b2, X[j].b3);
		set_k0(k, pass);
	}
}

void decrypt_k128(block_128 * X, block_128 * all_subkeys, block_128 * k, long number_of_blocks){
	int i, j;
	calc_all_ks(all_subkeys, k);
	for (j = 0; j < number_of_blocks; j++){
		printf("X[%d] B: %x %x %x %x\n",j, X[j].b0, X[j].b1, X[j].b2, X[j].b3);
		for (i = 11; i >= 0; i--)
		    X[j] = UmaIteracao_inv(X[j], calc_kr5(&all_subkeys[i]), calc_km32(&all_subkeys[i]));
		printf("X[%d] A: %x %x %x %x\n",j, X[j].b0, X[j].b1, X[j].b2, X[j].b3);
	}
}

/*	PRINTF TESTS 	*/

void tests(char* password, block_128 k, block_128 pass){
	printf("password: %s\n", password);
	printf("k: %x %x %x %x\n", k.b0, k.b1, k.b2, k.b3);
	printf("pass: %x %x %x %x\n", pass.b0, pass.b1, pass.b2, pass.b3);
	printf("s1: %x\n", s1[1]);
	printf("ConstR: %d\n", ConstR);
	printf("ConstM: %x\n", ConstM);
}


int main(int argc, char** argv){
	block_128 k, pass, km32;
	block_128 * X;
	block_32 kr5;
	block_128 * all_subkeys;
	int i, j;
	long file_size, number_of_blocks;
	k.b0 = 0x5A827999; 
	k.b1 = 0x874AA67D;
	k.b2 = 0x657B7C8E;
	k.b3 = 0xBD070242;
	read_sboxes();

	file_size = get_file_size(argv[1]);
    if (file_size % 16 == 0) number_of_blocks = file_size / 16;
    else number_of_blocks = file_size / 16 + 1;
    X = malloc (number_of_blocks * sizeof(block_128));
    all_subkeys = malloc (12 * sizeof(block_128));
    read_file(argv[1], X, number_of_blocks, file_size);

	pass_gen(argv[2], &pass, &k);

	encrypt_k128(X, &k, &pass, number_of_blocks);

    write_to_file("encrypt2.txt", X, number_of_blocks);

    decrypt_k128(X, all_subkeys, &k, number_of_blocks);
    write_to_file("dec.txt", X, number_of_blocks);
	
	return 0;
}