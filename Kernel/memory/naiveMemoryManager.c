#include "../include/memory/memoryManager.h"
#include <stddef.h>  

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
#define NEXT_PHYSICAL(block)  ((Block *)((char *)(block) + BLOCK_HDR_SZ + (block)->size))

static uint32_t hasRoomForSplit(Block *block, uint32_t requestedSize) {
    return block->size >= requestedSize + BLOCK_HDR_SZ + ALIGNMENT;
}

void createMemoryManager(void *memoryStartAddress, uint32_t memorySize) {
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
    for (Block *block = first; block; block = block->next)
        if (block->free && block->size >= requestedSize)
            return block;
    return NULL;
}

static void splitBlock(Block *block, uint32_t requestedSize) {
    Block *split = (Block *)((char *)block + BLOCK_HDR_SZ + requestedSize);

    split->size = block->size - requestedSize - BLOCK_HDR_SZ;
    split->free = 1;
    split->next = block->next;
    split->prev = block;

    if (block->next)
        block->next->prev = split;

    block->next = split;
    block->size = requestedSize;
}

void *allocMemory(uint32_t size) {
    if (size <= 0)
        return NULL;

    uint32_t requestedSize = ALIGN((uint32_t)size);

    Block *block = findFit(requestedSize);
    if (!block)
        return NULL;       /* out of memory */

    if (hasRoomForSplit(block, requestedSize))
        splitBlock(block, requestedSize);

    block->free = 0;
    return (void *)((char *)block + BLOCK_HDR_SZ);
}

static void mergeWithNext(Block *block) {
    Block *nxt = block->next;
    if (nxt && nxt->free && nxt == NEXT_PHYSICAL(block)) {
        block->size += BLOCK_HDR_SZ + nxt->size;
        block->next = nxt->next;
        if (nxt->next)
            nxt->next->prev = block;
    }
}

static void mergeWithPrev(Block *block) {
    Block *prv = block->prev;
    if (prv && prv->free && block == NEXT_PHYSICAL(prv)) {
        prv->size += BLOCK_HDR_SZ + block->size;
        prv->next  = block->next;
        if (block->next)
            block->next->prev = prv;
    }
}

static void coalesce(Block *block) {
    mergeWithNext(block);
    mergeWithPrev(block);
}

void freeMemory(void *memorySegment) {
    if (!memorySegment)
        return;

    Block *block = (Block *)((char *)memorySegment - BLOCK_HDR_SZ);
    block->free  = 1;
    coalesce(block);
}

/* memoryManager.c — mantengo todo tu código y solo modifico lo necesario */

/* … todo tu código sin cambios … */

void getMemoryStatus(MemoryStatus *status) {
    if (!status) 
        return;        

    uint32_t used  = 0;
    uint32_t freeb = 0;

    for (Block *block = first; block; block = block->next) {
        if (block->free)
            freeb += block->size;
        else
            used  += block->size;
    }

    status->total = poolSize;
    status->used  = used;
    status->free  = freeb;
    status->base  = (void *)first;
    status->end   = (void *)((Block *)first + poolSize);
}

