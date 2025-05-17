#ifndef MEMLIB_H
#define MEMLIB_H
<<<<<<< HEAD
#include <stdint.h>

void * malloc(uint32_t size);
void free(void * memorySegment);
=======

#include <stdint.h>
#include <stddef.h>

void *malloc(uint32_t size);
void free(void *memorySegment);
>>>>>>> parent of 7a25ff6 ([IMPORTANT] change kernel)

#endif