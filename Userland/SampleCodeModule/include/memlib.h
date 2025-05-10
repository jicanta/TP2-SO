#ifndef MEMLIB_H
#define MEMLIB_H

#include <stdint.h>
#include <stddef.h>

void *malloc(uint32_t size);
void free(void *memorySegment);

#endif