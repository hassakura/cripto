#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <limits.h>

#include "structs.h"
#include "utils.h"
#include "K128.h"




int main(int argc, char** argv){
	block_128 k, pass;
	block_128 * X;
	block_128 * all_subkeys;
	uint32_t number_of_blocks;
	uint32_t file_size; 
	k.b0 = 0x5A827999;
	k.b1 = 0x874AA67D;
	k.b2 = 0x657B7C8E;
	k.b3 = 0xBD070242;
	read_sboxes();

	file_size = get_file_size(argv[3]);
    if (file_size % 16 == 0){
    	number_of_blocks = file_size / 16 + 1;
    	X = malloc (number_of_blocks * sizeof(block_128));	
    	X[number_of_blocks - 1].b0 = file_size;
    }
    else{
    	number_of_blocks = file_size / 16 + 2;
    	X = malloc (number_of_blocks * sizeof(block_128));
    	X[number_of_blocks - 1].b0 = file_size;
    }
    
    all_subkeys = malloc (12 * sizeof(block_128));
    read_file(argv[3], X, number_of_blocks, file_size);
    
    if (strcmp(argv[1], "-c") == 0){
    	pass_gen(argv[7], &pass, &k);
    	encrypt_k128(X, &k, &pass, number_of_blocks);
    	write_to_file(argv[5], X, 16 * number_of_blocks);
    	if (argc > 8){
    		fopen(argv[3], "w");
    		if (remove(argv[3]) == 0)
    			printf("File removed\n");
    		else
    			printf("Error on removing file");
    	}
    }
    else if (strcmp(argv[1], "-d") == 0){
    	pass_gen(argv[7], &pass, &k);
    	decrypt_k128(X, all_subkeys, &k, &pass, number_of_blocks);
    	write_to_file(argv[5], X, X[number_of_blocks - 2].b0);
    }
    else if (strcmp(argv[1], "-1") == 0){
    	pass_gen(argv[5], &pass, &k);
    	hamming_K128(X, all_subkeys, &k, &pass, number_of_blocks, 0);
    }
    else if (strcmp(argv[1], "-2") == 0){
    	pass_gen(argv[5], &pass, &k);
    	hamming_K128(X, all_subkeys, &k, &pass, number_of_blocks, 1);
    }
    else
    	printf("Parametro nao reconhecido\n");

    free(X);
    free(all_subkeys);

	return 0;
}
