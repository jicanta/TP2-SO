// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#include "../include/defs.h"
#include <stdint.h>
#include <interrupts.h>

#define BLOCK_COUNT 250
#define ALIGNMENT 8

// Rounds up to the nearest multiple of the word size (8 bytes)
// If size is already a multiple of the word size, the bitwise operation keeps it unchanged
// Otherwise, it rounds up the size to the next multiple of the word size by adding (ALIGNMENT - 1)
// and then clearing the lower bits using a bitwise operation with ~(ALIGNMENT - 1)
//                    size   + 7 (111b)         & ~(111b)"
//                                              &   (000b)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct memHeader
{
    int size;
    int isFree;
    struct memHeader *next;
    struct memHeader *prev;
} memHeader;

memHeader *firstBlock;
int memSize, blockSize;
int bytesAllocated, bytesFree, blocksAllocated, bytesTotal;
void splitBlock(memHeader *block, int size);

void updateMemoryStats(int allocatedChange, int freeChange, int blockChange)
{
    bytesAllocated += allocatedChange;
    bytesFree += freeChange;
    blocksAllocated += blockChange;
}

void createMemoryManager(void *memoryStart, int memorySize)
{
    firstBlock = (memHeader *)memoryStart;
    firstBlock->size = memorySize - sizeof(memHeader);
    firstBlock->isFree = 1;
    firstBlock->next = NULL;
    firstBlock->prev = NULL;
    bytesTotal = memorySize;
    updateMemoryStats(sizeof(memHeader), firstBlock->size, 0);
}

void *allocMemory(int size)
{
    if (size <= 0)
    {
        return NULL;
    }
    size = ALIGN(size);
    memHeader *curr = firstBlock;
    while (curr != NULL && !(curr->isFree && curr->size >= size))
    {
        curr = curr->next;
    }
    if (curr == NULL)
    {
        return NULL;
    }
    if (curr->size >= size && (curr->size < (size + sizeof(memHeader))))
    {
        updateMemoryStats(curr->size, -curr->size, 1);
        curr->isFree = 0;
        return ((void *)(((void *)curr) + sizeof(memHeader)));
    }
    if (curr->size >= (size + sizeof(memHeader)))
    {

        splitBlock(curr, size);
        updateMemoryStats(size, -size, 1);
        return ((void *)(((void *)curr) + sizeof(memHeader)));
    }
    return NULL;
}

void freeMemory(void *memorySegment)
{

    if (memorySegment != NULL)
    {
        memHeader *curr = (memHeader *)(memorySegment - sizeof(memHeader));
        curr->isFree = 1;
        updateMemoryStats(-(curr->size), curr->size, -1);

        if (curr->prev != NULL && curr->prev->isFree)
        {
            curr->prev->size += (curr->size + sizeof(memHeader));
            curr->prev->next = curr->next;
            if (curr->next != NULL)
            {
                curr->next->prev = curr->prev;
            }
            curr = curr->prev;
            updateMemoryStats(-1 * (int)sizeof(memHeader), sizeof(memHeader), 0);
        }
        if (curr->next != NULL && curr->next->isFree)
        {
            updateMemoryStats(-1 * (int)sizeof(memHeader), sizeof(memHeader), 0);
            curr->size += (curr->next->size + sizeof(memHeader));
            curr->next = curr->next->next;
            if (curr->next != NULL)
            {
                curr->next->prev = curr;
            }
        }
    }
}

void splitBlock(memHeader *block, int size)
{
    memHeader *newBlock = NULL;
    if (bytesFree >= (size + sizeof(memHeader)))
    {
        newBlock = (memHeader *)((void *)block + size + sizeof(memHeader));
        newBlock->size = block->size - size - sizeof(memHeader);
        newBlock->isFree = 1;
        newBlock->next = block->next;
        newBlock->prev = block;
        updateMemoryStats(sizeof(memHeader), -1 * (int)sizeof(memHeader), 0);
        if (newBlock->next != NULL)
        {
            newBlock->next->prev = newBlock;
        }
    }
    block->size = size;
    block->isFree = 0;
    block->next = newBlock;
}

void getMemoryStatus(int *status)
{
    status[0] = blocksAllocated;
    status[1] = bytesAllocated;
    status[2] = bytesFree;
    status[3] = bytesFree + bytesAllocated;
}