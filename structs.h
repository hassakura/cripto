#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <stdint.h>


typedef uint32_t sbox [256];

typedef struct block128bits{
	uint32_t b0;
	uint32_t b1;
	uint32_t b2;
	uint32_t b3;
} block_128;

typedef struct block32bits{
	uint8_t b0;
	uint8_t b1;
	uint8_t b2;
	uint8_t b3;
} block_32;

#endif /* STRUCTS_H_ */
