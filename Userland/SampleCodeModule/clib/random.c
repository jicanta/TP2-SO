#include <random.h>

static uint8_t seed;

void srand(uint8_t new_seed) {
  seed = new_seed;
}

uint8_t rand() {
  seed = (seed * MULT + INCR) % 256;
  return seed;
}

uint8_t getSecMinSeed() {
  time_struct time;
  lib_get_time(&time);
  return (time.seconds ^ (time.minutes * 60)) % 256;
}