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

    write_to_file("encrypt_main.txt", X, number_of_blocks);

    decrypt_k128(X, all_subkeys, &k, &pass, number_of_blocks);
    write_to_file("decrypt_main.txt", X, number_of_blocks);

    hamming_K128(X, all_subkeys, &k, &pass, number_of_blocks, 0);
    hamming_K128(X, all_subkeys, &k, &pass, number_of_blocks, 1);

    free(X);
    free(all_subkeys);

	return 0;
}
