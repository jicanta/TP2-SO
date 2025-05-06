#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

void createMemoryManager(void *memoryStartAddress, int memorySize);

void *allocMemory(int size);

void freeMemory(void *memorySegment);

void getMemoryStatus(int *status);
#endif