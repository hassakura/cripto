#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>

/* 	uint8_t = 	1 byte 	= 8  bits
	uint32_t = 	4 bytes	= 32 bits
	uint64_t = 	8 bytes = 64 bits
						*/

#define MOD32 4294967296

/*	TYPEDEFS	*/

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

/* -------------------------------------------- */

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

void pass_gen (char* old_pass, block_128 * pass){
	size_t pass_len = strlen(old_pass);
	char* new_pass = malloc(16 * sizeof (uint8_t));
	
	
	if (pass_len < 16){
		memcpy(new_pass, old_pass, pass_len);
		memcpy(new_pass + pass_len, old_pass, 16 - pass_len);
	} 
	else memcpy(new_pass, old_pass, 16);
	
	pass->b0 = convert_string_to_uint (new_pass);
	pass->b1 = convert_string_to_uint (new_pass + 4);
	pass->b2 = convert_string_to_uint (new_pass + 8);
	pass->b3 = convert_string_to_uint (new_pass + 12);

	printf("pass: %x%x%x%x\n", pass->b0, pass->b1, pass->b2, pass->b3);
}

/*	INTERM KEYS	*/

void calc_k5(int i, block_32 * k5){ 
	k5->b0 = rotate_left_8(ConstR, ((int) pow(i + 2, 2) % 3));
	k5->b1 = rotate_left_8(ConstR, ((int) pow(i + 2, 1) % 3));
	k5->b2 = rotate_left_8(ConstR, ((int) pow(i + 2, 3) % 3));
	k5->b3 = rotate_left_8(ConstR, ((int) pow(i + 2, 2) % 3));
}


void calc_k32(int i, block_128 * k32){
	k32->b0 = rotate_left_32(ConstM, ((int) pow(i + 3, 2) % 7));
	k32->b1 = rotate_left_32(ConstM, ((int) pow(i + 3, 1) % 7));
	k32->b2 = rotate_left_32(ConstM, ((int) pow(i + 3, 3) % 7));
	k32->b3 = rotate_left_32(ConstM, ((int) pow(i + 3, 2) % 7));
}

void calc_k(int iteration, block_128 * k, block_128 * k32, block_32 * k5){

	calc_k5(iteration, k5);
	calc_k32(iteration, k32);

	printf("k5:%d %d %d %d\n", k5->b0, k5->b1, k5->b2, k5->b3);
	printf("k32:%x %x %x %x\n", k32->b0, k32->b1, k32->b2, k32->b3);
	k->b0 = k->b0 ^ calc_f2(k->b3, k5->b0, k32->b0);
	k->b1 = k->b1 ^ calc_f1(k->b0, k5->b1, k32->b1);
	k->b2 = k->b2 ^ calc_f3(k->b1, k5->b2, k32->b2);
	k->b3 = k->b3 ^ calc_f2(k->b2, k5->b3, k32->b3);
	printf("k: %x %x %x %x\n", k->b0, k->b1, k->b2, k->b3);
}


/*	SUBKEYS KR5 & KM32 	*/


block_32 calc_kr5(block_128 * k){

	block_32 kr5;
	kr5.b0 = last_5_bits(k->b3);
	kr5.b1 = last_5_bits(k->b2);
	kr5.b2 = last_5_bits(k->b1);
	kr5.b3 = last_5_bits(k->b0);
	printf("kr5: %d %d %d %d\n", kr5.b0, kr5.b1, kr5.b2, kr5.b3);
	return kr5;
}

block_128 calc_km32(block_128 * k){
	block_128 km32;
	km32.b0 = k->b0;
	km32.b1 = k->b1;
	km32.b2 = k->b2;
	km32.b3 = k->b3;
	printf("km32: %x %x %x %x\n", km32.b0, km32.b1, km32.b2, km32.b3);
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


block_128 UmaIteracao(int iteration, block_128 X, block_32 kr5, block_128 km32){
	/* K deve ser "global" */
	
	X.b2 = X.b2 ^ calc_f2(X.b3, kr5.b0, km32.b0);
	X.b1 = X.b1 ^ calc_f1(X.b2, kr5.b1, km32.b1);
	X.b0 = X.b0 ^ calc_f3(X.b1, kr5.b2, km32.b2);
	X.b3 = X.b3 ^ calc_f2(X.b0, kr5.b3, km32.b3);
}

/*	PRINTF TESTS 	*/

void tests(char* password, block_128 k, block_128 pass, block_32 k5, block_128 k32, uint32_t f1, uint32_t f2, uint32_t f3){
	printf("password: %s\n", password);
	printf("k: %x%x%x%x\n", k.b0, k.b1, k.b2, k.b3);
	printf("pass: %x%x%x%x\n", pass.b0, pass.b1, pass.b2, pass.b3);
	printf("s1: %x\n", s1[1]);
	printf("ConstR: %d\n", ConstR);
	printf("ConstM: %x\n", ConstM);
	printf("k5:%d %d %d %d\n", k5.b0, k5.b1, k5.b2, k5.b3);
	printf("k32:%x %x %x %x\n", k32.b0, k32.b1, k32.b2, k32.b3);
	printf("f1: %x\n", f1);
	printf("f2: %x\n", f2);
	printf("f3: %x\n", f3);
}



int main(int argc, char** argv){
	block_128 k, pass, k32, km32;
	block_32 k5, kr5;
	uint32_t f1, f2, f3;
	k.b0 = 0x5A827999; 
	k.b1 = 0x874AA67D;
	k.b2 = 0x657B7C8E;
	k.b3 = 0xBD070242;
	read_sboxes();
	pass_gen(argv[1], &pass);
	calc_k5(1, &k5);
	calc_k32(1, &k32);
	f1 = calc_f1(k.b0, k5.b0, k32.b0);
	f2 = calc_f1(k.b3, k5.b1, k32.b1);
	f3 = calc_f1(k.b1, k5.b2, k32.b2);
	tests(argv[1], k, pass, k5, k32, f1, f2, f3);
	calc_k(3, &k, &k32, &k5);
    kr5 = calc_kr5(&k);
    km32 = calc_km32(&k);
	/*printf("xor:\nk0_1 xor pass1: %lx\nk0_2 xor pass2: %lx\nAll: %lx%lx\n", k0_1 ^ convert_string_to_uint(pass), 
		k0_2 ^ convert_string_to_uint(pass + 8), k0_1 ^ convert_string_to_uint(pass), k0_2 ^ convert_string_to_uint(pass+ 8));*/
	
	return 0;
}