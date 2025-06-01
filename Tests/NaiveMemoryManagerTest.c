#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"
#include "MemoryManagerTest.h"    

typedef struct Block {
    uint32_t       size;
    uint8_t        free;
    struct Block  *next;
    struct Block  *prev;
} Block;

static Block   *firstBlock = NULL;
static uint32_t memoryPoolSize = 0;

#define ALIGNMENT   8u
#define ALIGN(sz)   (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define MANAGED_MEMORY_SIZE  2048u
#define ALLOCATION_SIZE      64u

#define BLOCK_HEADER_SIZE  ((uint32_t)ALIGN(sizeof(Block)))
#define TO_BYTE_PTR(ptr)   ((uint8_t *)(ptr))

#define NEXT_PHYSICAL_BLOCK(block) \
    ((Block *)(TO_BYTE_PTR(block) + BLOCK_HEADER_SIZE + (block)->size))

static uint32_t hasRoomForSplit(Block *block, uint32_t requestedSize) {
    return block->size >= requestedSize + BLOCK_HEADER_SIZE + ALIGNMENT;
}

void createMemoryManager(void *memoryStartAddress, uint32_t memorySize) {
    if (!memoryStartAddress || memorySize <= BLOCK_HEADER_SIZE)
        return;

    firstBlock       = (Block *)memoryStartAddress;
    firstBlock->size = memorySize - BLOCK_HEADER_SIZE;
    firstBlock->free = 1;
    firstBlock->next = NULL;
    firstBlock->prev = NULL;

    memoryPoolSize   = memorySize;
}


static Block *findSuitableBlock(uint32_t requestedSize) {
    for (Block *block = firstBlock; block; block = block->next)
        if (block->free && block->size >= requestedSize)
            return block;
    return NULL;
}

static void splitBlock(Block *block, uint32_t requestedSize) {
    Block *newBlock = (Block *)(TO_BYTE_PTR(block) + BLOCK_HEADER_SIZE + requestedSize);

    newBlock->size = block->size - requestedSize - BLOCK_HEADER_SIZE;
    newBlock->free = 1;
    newBlock->next = block->next;
    newBlock->prev = block;

    if (block->next)
        block->next->prev = newBlock;

    block->next = newBlock;
    block->size = requestedSize;
}

void *allocMemory(uint32_t size) {
    if (size == 0)
        return NULL;

    uint32_t alignedSize = ALIGN(size);
    Block *block = findSuitableBlock(alignedSize);
    if (!block)
        return NULL;

    if (hasRoomForSplit(block, alignedSize))
        splitBlock(block, alignedSize);

    block->free = 0;
    return TO_BYTE_PTR(block) + BLOCK_HEADER_SIZE;
}

static void mergeWithNext(Block *block) {
    Block *nextBlock = block->next;
    if (nextBlock && nextBlock->free && nextBlock == NEXT_PHYSICAL_BLOCK(block)) {
        block->size += BLOCK_HEADER_SIZE + nextBlock->size;
        block->next  = nextBlock->next;
        if (nextBlock->next)
            nextBlock->next->prev = block;
    }
}

static void mergeWithPrevious(Block *block) {
    Block *previousBlock = block->prev;
    if (previousBlock && previousBlock->free && block == NEXT_PHYSICAL_BLOCK(previousBlock)) {
        previousBlock->size += BLOCK_HEADER_SIZE + block->size;
        previousBlock->next  = block->next;
        if (block->next)
            block->next->prev = previousBlock;
    }
}

static void coalesce(Block *block) {
    mergeWithNext(block);
    mergeWithPrevious(block);
}

void freeMemory(void *memorySegment) {
    if (!memorySegment)
        return;

    Block *block = (Block *)(TO_BYTE_PTR(memorySegment) - BLOCK_HEADER_SIZE);
    if (block->free) return;

    block->free = 1;
    coalesce(block);
}


void getMemoryStatus(MemoryStatus *status) {
    if (!status)
        return;

    uint32_t usedBytes = 0;
    uint32_t freeBytes = 0;

    for (Block *block = firstBlock; block; block = block->next) {
        if (block->free)
            freeBytes += block->size;
        else
            usedBytes += block->size;
    }

    status->total = memoryPoolSize;
    status->used  = usedBytes;
    status->free  = freeBytes;
    status->base  = (void *)firstBlock;
    status->end   = (void *)(TO_BYTE_PTR(firstBlock) + memoryPoolSize);
}

static void *createPool(void) {
    void *pool = malloc(MANAGED_MEMORY_SIZE);
    CuAssertPtrNotNullMsg(NULL, "malloc failed in test", pool);
    createMemoryManager(pool, MANAGED_MEMORY_SIZE);
    return pool;
}


static void testInitialization(CuTest *tc) {
    void *pool = createPool();

    MemoryStatus st;
    getMemoryStatus(&st);

    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE,                    st.total);
    CuAssertIntEquals(tc, 0,                                      st.used);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE - BLOCK_HEADER_SIZE,st.free);
    CuAssertPtrEquals(tc, pool,                                   st.base);
    CuAssertPtrEquals(tc, (uint8_t*)pool + MANAGED_MEMORY_SIZE,   st.end);

    free(pool);
}

static void testSingleAllocation(CuTest *tc) {
    void *pool = createPool();

    void *ptr = allocMemory(ALLOCATION_SIZE);
    CuAssertPtrNotNull(tc, ptr);
    CuAssertTrue(tc, ((uintptr_t)ptr % ALIGNMENT) == 0); 

    MemoryStatus st;
    getMemoryStatus(&st);

    CuAssertTrue(tc, ptr >= st.base && ptr < st.end);
    CuAssertIntEquals(tc,
        ALIGN(ALLOCATION_SIZE),          
        st.used);                        

    free(pool);
}

static void testFreeAndCoalesce(CuTest *tc) {
    void *pool = createPool();

    void *p1 = allocMemory(32);
    void *p2 = allocMemory(48);
    CuAssertPtrNotNull(tc, p1);
    CuAssertPtrNotNull(tc, p2);

    freeMemory(p1);
    freeMemory(p2);           

    MemoryStatus st;
    getMemoryStatus(&st);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE - BLOCK_HEADER_SIZE, st.free);

    free(pool);
}

static void testOutOfMemory(CuTest *tc) {
    void *pool = createPool();

    void *big = allocMemory(MANAGED_MEMORY_SIZE);  
    CuAssertPtrEquals(tc, NULL, big);

    void *full = allocMemory(MANAGED_MEMORY_SIZE - 2*BLOCK_HEADER_SIZE);
    CuAssertPtrNotNull(tc, full);

    void *fail = allocMemory(ALIGNMENT);            
    CuAssertPtrEquals(tc, NULL, fail);

    free(pool);
}

static void testMultipleAllocations(CuTest *tc) {
    void *pool = createPool();

    void *p[4];
    for (int i=0;i<4;i++)
        p[i] = allocMemory(ALLOCATION_SIZE);

    for (int i=0;i<4;i++)
        CuAssertPtrNotNull(tc, p[i]);

    for (int i=1;i<4;i++)
        CuAssertTrue(tc,
            (uint8_t*)p[i-1] + ALIGN(ALLOCATION_SIZE) + BLOCK_HEADER_SIZE <= (uint8_t*)p[i]);

    for (int i=3;i>=0;i--)
        freeMemory(p[i]);

    MemoryStatus st;
    getMemoryStatus(&st);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE - BLOCK_HEADER_SIZE, st.free);

    free(pool);
}

CuSuite *getNaiveMemoryManagerTestSuite(void) {
    CuSuite *suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testInitialization);
    SUITE_ADD_TEST(suite, testSingleAllocation);
    SUITE_ADD_TEST(suite, testFreeAndCoalesce);
    SUITE_ADD_TEST(suite, testOutOfMemory);
    SUITE_ADD_TEST(suite, testMultipleAllocations);
    return suite;
}