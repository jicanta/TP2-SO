#include <stdint.h>
#include <stdlib.h>
#include "CuTest.h"
#include "DummyTest.h"
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



void testMemoryManagerInitialization(CuTest *const cuTest);
void testMemoryAllocation(CuTest *const cuTest);
void testMemoryFree(CuTest *const cuTest);
void testMemoryBoundaries(CuTest *const cuTest);
void testMultipleAllocations(CuTest *const cuTest);

static const size_t TestQuantity = 5;
static const Test MemoryManagerTests[] = {
    testMemoryManagerInitialization,
    testMemoryAllocation,
    testMemoryFree,
    testMemoryBoundaries,
    testMultipleAllocations
};

// Test variables
static void *managedMemory;
static const uint32_t MANAGED_MEMORY_SIZE = 2048;
static const uint32_t ALLOCATION_SIZE = 64;
static void *allocatedMemory = NULL;

CuSuite *getMemoryManagerTestSuite(void) {
    CuSuite *const suite = CuSuiteNew();

    for (size_t i = 0; i < TestQuantity; i++)
        SUITE_ADD_TEST(suite, MemoryManagerTests[i]);

    return suite;
}

void testMemoryManagerInitialization(CuTest *const cuTest) {
    managedMemory = malloc(MANAGED_MEMORY_SIZE);
    if (managedMemory == NULL) {
        CuFail(cuTest, "Failed to allocate managed memory");
    }

    createMemoryManager(managedMemory, MANAGED_MEMORY_SIZE);
    
    MemoryStatus status;
    getMemoryStatus(&status);
    
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE, status.total);
    CuAssertIntEquals(cuTest, 0, status.used);
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE - BLOCK_HEADER_SIZE, status.free);
    CuAssertPtrEquals(cuTest, managedMemory, status.base);
    CuAssertPtrEquals(cuTest, (Block *)managedMemory + MANAGED_MEMORY_SIZE, status.end);
}

void testMemoryAllocation(CuTest *const cuTest) {
    managedMemory = malloc(MANAGED_MEMORY_SIZE);
    if (managedMemory == NULL) {
        CuFail(cuTest, "Failed to allocate managed memory");
    }
    createMemoryManager(managedMemory, MANAGED_MEMORY_SIZE);

    allocatedMemory = allocMemory(ALLOCATION_SIZE);
    CuAssertPtrNotNull(cuTest, allocatedMemory);
    
    MemoryStatus status;
    getMemoryStatus(&status);
    CuAssertTrue(cuTest, allocatedMemory >= status.base);
    CuAssertTrue(cuTest, (char *)allocatedMemory + ALLOCATION_SIZE <= (char *)status.end);
}

void testMemoryFree(CuTest *const cuTest) {
    managedMemory = malloc(MANAGED_MEMORY_SIZE);
    if (managedMemory == NULL) {
        CuFail(cuTest, "Failed to allocate managed memory");
    }
    createMemoryManager(managedMemory, MANAGED_MEMORY_SIZE);

    void *ptr = allocMemory(ALLOCATION_SIZE);
    CuAssertPtrNotNull(cuTest, ptr);
    
    freeMemory(ptr);
    
    MemoryStatus status;
    getMemoryStatus(&status);
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE - BLOCK_HEADER_SIZE, status.free);
}

void testMemoryBoundaries(CuTest *const cuTest) {
    managedMemory = malloc(MANAGED_MEMORY_SIZE);
    if (managedMemory == NULL) {
        CuFail(cuTest, "Failed to allocate managed memory");
    }
    createMemoryManager(managedMemory, MANAGED_MEMORY_SIZE);
    
    void *firstAlloc = allocMemory(ALLOCATION_SIZE);
    CuAssertPtrNotNull(cuTest, firstAlloc);
    
    void *lastAlloc = allocMemory(MANAGED_MEMORY_SIZE - ALLOCATION_SIZE - 100);
    CuAssertPtrNotNull(cuTest, lastAlloc);
    
    MemoryStatus status;
    getMemoryStatus(&status);
    CuAssertTrue(cuTest, firstAlloc >= status.base);
    CuAssertTrue(cuTest, lastAlloc >= status.base);
    CuAssertTrue(cuTest, (char *)firstAlloc + ALLOCATION_SIZE <= (char *)status.end);
    CuAssertTrue(cuTest, (char *)lastAlloc + (MANAGED_MEMORY_SIZE - ALLOCATION_SIZE - 100) <= (char *)status.end);
}

void testMultipleAllocations(CuTest *const cuTest) {
    managedMemory = malloc(MANAGED_MEMORY_SIZE);
    if (managedMemory == NULL) {
        CuFail(cuTest, "Failed to allocate managed memory");
    }
    createMemoryManager(managedMemory, MANAGED_MEMORY_SIZE);
    
    void *ptr1 = allocMemory(ALLOCATION_SIZE);
    void *ptr2 = allocMemory(ALLOCATION_SIZE);
    void *ptr3 = allocMemory(ALLOCATION_SIZE);
    
    CuAssertPtrNotNull(cuTest, ptr1);
    CuAssertPtrNotNull(cuTest, ptr2);
    CuAssertPtrNotNull(cuTest, ptr3);
    
    CuAssertTrue(cuTest, (char *)ptr1 + ALLOCATION_SIZE + BLOCK_HEADER_SIZE <= (char *)ptr2);
    CuAssertTrue(cuTest, (char *)ptr2 + ALLOCATION_SIZE + BLOCK_HEADER_SIZE <= (char *)ptr3);
    
    freeMemory(ptr1);
    freeMemory(ptr2);
    freeMemory(ptr3);
    
    MemoryStatus status;
    getMemoryStatus(&status);
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE - BLOCK_HEADER_SIZE, status.free);
}