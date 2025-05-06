#include "../include/memory/memoryManager.h"
#include <stddef.h>  
#include <stdint.h>

typedef struct Block {
    uint32_t       size;
    uint8_t        free;
    struct Block  *next; 
    struct Block  *prev;
} Block;


static Block   *first    = NULL;
static uint32_t poolSize = 0;

#define ALIGNMENT      8
#define ALIGN(sz)      (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define BLOCK_HDR_SZ   ((uint32_t)ALIGN(sizeof(Block)))
#define NEXT_PHYSICAL(b)  ((Block *)((char *)(b) + BLOCK_HDR_SZ + (b)->size))

static int hasRoomForSplit(Block *b, uint32_t requestedSize) {
    return b->size >= requestedSize + BLOCK_HDR_SZ + ALIGNMENT;
}

void createMemoryManager(void *memoryStartAddress, int memorySize) {
    if (memorySize <= (int)BLOCK_HDR_SZ)       
        return;

    first            = (Block *)memoryStartAddress;
    first->size      = memorySize - BLOCK_HDR_SZ;
    first->free      = 1;
    first->next      = NULL;
    first->prev      = NULL;

    poolSize         = memorySize;
}


static Block *findFit(uint32_t requestedSize) {
    for (Block *b = first; b; b = b->next)
        if (b->free && b->size >= requestedSize)
            return b;
    return NULL;
}

static void splitBlock(Block *b, uint32_t requestedSize) {
    Block *split = (Block *)((char *)b + BLOCK_HDR_SZ + requestedSize);

    split->size = b->size - requestedSize - BLOCK_HDR_SZ;
    split->free = 1;
    split->next = b->next;
    split->prev = b;

    if (b->next)
        b->next->prev = split;

    b->next = split;
    b->size = requestedSize;
}

void *allocMemory(int size) {
    if (size <= 0)
        return NULL;

    uint32_t requestedSize = ALIGN((uint32_t)size);

    Block *b = findFit(requestedSize);
    if (!b)
        return NULL;       /* out of memory */

    if (hasRoomForSplit(b, requestedSize))
        splitBlock(b, requestedSize);

    b->free = 0;
    return (void *)((char *)b + BLOCK_HDR_SZ);
}

static void mergeWithNext(Block *b) {
    Block *nxt = b->next;
    if (nxt && nxt->free && nxt == NEXT_PHYSICAL(b)) {
        b->size += BLOCK_HDR_SZ + nxt->size;
        b->next = nxt->next;
        if (nxt->next)
            nxt->next->prev = b;
    }
}

static void mergeWithPrev(Block *b) {
    Block *p = b->prev;
    if (p && p->free && b == NEXT_PHYSICAL(p)) {
        p->size += BLOCK_HDR_SZ + b->size;
        p->next  = b->next;
        if (b->next)
            b->next->prev = p;
    }
}

static void coalesce(Block *b) {
    mergeWithNext(b);
    mergeWithPrev(b);
}

void freeMemory(void *memorySegment) {
    if (!memorySegment)
        return;

    Block *b = (Block *)((char *)memorySegment - BLOCK_HDR_SZ);
    b->free  = 1;
    coalesce(b);
}

void getMemoryStatus(int *status) {
    uint32_t used = 0, freeb = 0;

    for (Block *b = first; b; b = b->next) {
        if(b->free)
            freeb += b->size;
        else
            used += b->size;
    }

    status[0] = (int)poolSize;
    status[1] = (int)used;
    status[2] = (int)freeb;
}
