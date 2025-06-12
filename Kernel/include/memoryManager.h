#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include "defs.h"

#define ALIGNMENT      8u
#define ALIGN(sz)      (((sz) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

/*
 * Initializes the memory manager with a given memory region.
 * Parameters:
 *   memoryStartAddress - The starting address of the memory region.
 *   memorySize - The size of the memory region in bytes.
 */
void createMemoryManager(void *memoryStartAddress, uint32_t memorySize);

/*
 * Allocates a block of memory of the specified size.
 * Parameters:
 *   size - The size of the memory block to allocate in bytes.
 * Returns a pointer to the allocated memory block, or NULL if allocation fails.
 */
void *allocMemory(uint32_t size);

/*
 * Frees a previously allocated memory block.
 * Parameters:
 *   memorySegment - Pointer to the memory block to free.
 */
void freeMemory(void *memorySegment);

/*
 * Retrieves the current status of the memory manager.
 * Parameters:
 *   status - Pointer to a MemoryStatus structure to store the status information.
 */
void getMemoryStatus(MemoryStatus *status);
#endif