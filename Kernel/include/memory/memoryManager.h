#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>

void createMemoryManager(void *memoryStartAddress, uint32_t memorySize);

void *allocMemory(uint32_t size);

void freeMemory(void *memorySegment);

void getMemoryStatus(uint32_t *status);
#endif