#include "K128.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define MOD32 4294967296 /*	2^32 */

sbox s1, s2, s3, s4;
uint8_t ConstR = 0b0010011;
uint32_t ConstM = 0xCB3725F7;


/*	Le os arquivos sbox1..sbox4 e guarda os valores das sboxes
	em s1..s4 	*/
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

/*	Recebe a senha inserida pelo usuario e calcula o valor
	de k0 (K0 = K XOR (5A827999874AA67D657B7C8EBD070242)16)	*/

void set_k0(block_128 * k, block_128 * pass){
	k->b0 = 0x5A827999 ^ pass->b0;
	k->b1 = 0x874AA67D ^ pass->b1;
	k->b2 = 0x657B7C8E ^ pass->b2;
	k->b3 = 0xBD070242 ^ pass->b3;
}


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


/* 	Recebe a senha do usuario, e se len(pass) < 16, concatena com os
	mesmos caracteres de pass ate que len(pass) - 16. No final, faz
	a conversao de string para valores inteiros
*/
void pass_gen (char* old_pass, block_128 * pass, block_128 * k){
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

	set_k0(k, pass);

	free(new_pass);

}



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


	X.b2 = X.b2 ^ calc_f2(X.b3, kr5.b0, km32.b0);
	X.b1 = X.b1 ^ calc_f1(X.b2, kr5.b1, km32.b1);
	X.b0 = X.b0 ^ calc_f3(X.b1, kr5.b2, km32.b2);
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

/*	Calcula todos os valores dos Ks, usando a senha dada.
	Usado para a decriptografia
*/

void calc_all_ks(block_128 * all_subkeys, block_128 * k){
	int i;
	for (i = 0; i < 12; i++){
		calc_k(i, k);
		memcpy (&all_subkeys[i], k, sizeof(block_128));
	}
}



void encrypt_k128(block_128 * X, block_128 * k, block_128 * pass, uint32_t number_of_blocks){
	int i, j;
	block_128 VI; /* VI dado pelo enunciado (todos bits 1)	*/
	X[0].b0 ^= 0xFFFFFFFF;
	X[0].b1 ^= 0xFFFFFFFF;
	X[0].b2 ^= 0xFFFFFFFF;
	X[0].b3 ^= 0xFFFFFFFF;
	for (j = 0; j < number_of_blocks; j++){
		for (i = 0; i < 12; i++){
			calc_k(i, k);
		    X[j] = UmaIteracao(X[j], calc_kr5(k), calc_km32(k));
		}
		set_k0(k, pass);
		/*	Operacao do modo CBC (Yk = fk(x ^ Yk-1))	*/
		if (j + 1 < number_of_blocks){
			X[j + 1].b0 ^= X[j].b0;
			X[j + 1].b1 ^= X[j].b1;
			X[j + 1].b2 ^= X[j].b2;
			X[j + 1].b3 ^= X[j].b3;
		}
	}
}

void decrypt_k128(block_128 * X, block_128 * all_subkeys, block_128 * k, block_128 * pass, uint32_t number_of_blocks){
	int i, j;
	block_128 VI;
	block_128 * X_CBC;
	VI.b0 = 0xFFFFFFFF;
	VI.b1 = 0xFFFFFFFF;
	VI.b2 = 0xFFFFFFFF;
	VI.b3 = 0xFFFFFFFF;

	X_CBC = malloc (number_of_blocks * sizeof(block_128));
	memcpy(X_CBC, X, number_of_blocks * sizeof(block_128));

	calc_all_ks(all_subkeys, k);
	for (j = 0; j < number_of_blocks; j++){
		for (i = 11; i >= 0; i--)
		    X[j] = UmaIteracao_inv(X[j], calc_kr5(&all_subkeys[i]), calc_km32(&all_subkeys[i]));


	}
	/*	Operacao do modo CBC, mas na ordem inversa (Xk = fk(Yk) ^ Yk-1)	*/
	for (j = 0; j < number_of_blocks; j++){
		if (j){
				X[j].b0 ^= X_CBC[j - 1].b0;
				X[j].b1 ^= X_CBC[j - 1].b1;
				X[j].b2 ^= X_CBC[j - 1].b2;
				X[j].b3 ^= X_CBC[j - 1].b3;
			}
			else{
				X[j].b0 ^= VI.b0;
				X[j].b1 ^= VI.b1;
				X[j].b2 ^= VI.b2;
				X[j].b3 ^= VI.b3;
		}
	}
	free(X_CBC);
	set_k0(k, pass);
}

/*	Calcula a distancia de Hamming entre o bloco (128 bits) de VetEntraC e VetAlterC */
int hamming_dist(block_128 BlC, block_128 BlAC){

	block_128 diff_bits;
	int dist = 0;


	for (diff_bits.b0 = BlC.b0 ^ BlAC.b0; diff_bits.b0 > 0; diff_bits.b0 >>= 1)
		if (diff_bits.b0 & 1) dist++;
	for (diff_bits.b1 = BlC.b1 ^ BlAC.b1; diff_bits.b1 > 0; diff_bits.b1 >>= 1)
		if (diff_bits.b1 & 1) dist++;
	for (diff_bits.b2 = BlC.b2 ^ BlAC.b2; diff_bits.b2 > 0; diff_bits.b2 >>= 1)
		if (diff_bits.b2 & 1) dist++;
	for (diff_bits.b3 = BlC.b3 ^ BlAC.b3; diff_bits.b3 > 0; diff_bits.b3 >>= 1)
		if (diff_bits.b3 & 1) dist++;

	return dist;
}


/*	Recebe o conteudo do arquivo lido, e calcula a distancia de hammming como
	descrito no enunciado. Apos o calculo, mostra os valores maximos, minimos
	e a media de cada bloco (128 bits). Se flag == 0, muda apenas o valor do
	j-esimo bit, senao, muda tambem o valor do (j + 8)-esimo bit.
*/

void hamming_K128(block_128 * X, block_128 * all_subkeys, block_128 * k, block_128 * pass, uint32_t number_of_blocks, int flag){

	block_128 * VetAlter;
	block_128 * VetEntra;
	int i, j, l, h_dist = 0;

	VetEntra = malloc (number_of_blocks * sizeof(block_128));
    VetAlter = malloc (number_of_blocks * sizeof(block_128));

	memcpy(VetAlter, X, number_of_blocks * sizeof(block_128));
	memcpy(VetEntra, X, number_of_blocks * sizeof(block_128));

	encrypt_k128(VetEntra, k, pass, number_of_blocks);

	/*	Começa o barato */

	long SomaH [number_of_blocks];
	long MinH [number_of_blocks];
	long MaxH [number_of_blocks];

	for (i = 0; i < number_of_blocks; i++){
		SomaH[i] = 0;
		MinH[i] = INT_MAX;
		MaxH[i] = 0;
	}

	for (j = 0; j < 128 * number_of_blocks; j++){
		change_bit_in_position(j, &VetAlter[j/128]);
		if (flag && j + 8 < (128 * number_of_blocks)) change_bit_in_position(j + 8, &VetAlter[(j + 8) / 128]);
		encrypt_k128(VetAlter, k, pass, number_of_blocks);
		for (l = 0; l < number_of_blocks;l++){
			h_dist = hamming_dist(VetEntra[l], VetAlter[l]);
			if (h_dist > MaxH[l]) MaxH[l] = h_dist;
			if (h_dist < MinH[l] && h_dist) MinH[l] = h_dist;
			SomaH[l] += h_dist;
		}
		decrypt_k128(VetAlter, all_subkeys, k, pass, number_of_blocks);

		change_bit_in_position(j, &VetAlter[j/128]);
		if (flag && j + 8 < (128 * number_of_blocks)) change_bit_in_position(j + 8, &VetAlter[(j + 8) / 128]);

	}
	for (j = 0; j < number_of_blocks; j++)
		printf("[%d]: Min: %ld Max: %ld Media: %f\n",j, MinH[j], MaxH[j], SomaH[j] / (128.0 * (j + 1)));
	free(VetEntra);
	free(VetAlter);
}

