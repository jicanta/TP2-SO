#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <lib.h>

// Constants
#define MULT 85
#define INCR 1

void srand(uint8_t new_seed);

uint8_t rand();

uint8_t getSecMinSeed();

#endif