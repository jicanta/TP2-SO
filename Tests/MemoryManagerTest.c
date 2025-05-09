#include <stdint.h>
#include <stdlib.h>
#include "CuTest.h"
#include "DummyTest.h"
#include "MemoryManagerTest.h"

#define ALIGNMENT 8
#define ALIGN(sz) (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define BLOCK_HDR_SZ ((uint32_t)ALIGN(sizeof(Block)))

typedef struct Block {
    uint32_t       size;
    uint8_t        free;
    struct Block  *next; 
    struct Block  *prev;
} Block;


static Block   *first    = NULL;
static uint32_t poolSize = 0;

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
        return NULL;     

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
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE - BLOCK_HDR_SZ, status.free);
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
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE - BLOCK_HDR_SZ, status.free);
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
    
    CuAssertTrue(cuTest, (char *)ptr1 + ALLOCATION_SIZE + BLOCK_HDR_SZ <= (char *)ptr2);
    CuAssertTrue(cuTest, (char *)ptr2 + ALLOCATION_SIZE + BLOCK_HDR_SZ <= (char *)ptr3);
    
    freeMemory(ptr1);
    freeMemory(ptr2);
    freeMemory(ptr3);
    
    MemoryStatus status;
    getMemoryStatus(&status);
    CuAssertIntEquals(cuTest, MANAGED_MEMORY_SIZE - BLOCK_HDR_SZ, status.free);
}