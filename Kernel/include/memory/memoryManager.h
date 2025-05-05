#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

void memoryManagerInit(void *memoryStartAddress, int memorySize);

void *memoryManagerMalloc(int size);

void memoryManagerFree(void *memorySegment);

void getMemoryStatus(int *status);
#endif