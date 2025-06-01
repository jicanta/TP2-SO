#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"
#include "MemoryManagerTest.h"         


#define ALIGNMENT         8u
#define ALIGN(sz)        (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct {
    uint8_t order;
    uint8_t _pad[7];
} AllocHdr;

#define HDR_SIZE          ((uint32_t)sizeof(AllocHdr))

/* pool y tallas usadas para las pruebas */
#define MANAGED_MEMORY_SIZE  2048u      /* 2 KiB – potencia de dos      */
#define ALLOCATION_SIZE      64u

#define MIN_BLOCK_SIZE ALIGN(HDR_SIZE + ALIGNMENT)

enum { FREE = 0, SPLIT = 1, FULL = 2 };

static uint8_t  *poolBase   = NULL;
static uint32_t poolSize    = 0;      
static uint32_t maxOrder    = 0;      

#define MAX_ORDER_ALLOWED  20         
static uint8_t   nodeState[(1u << (MAX_ORDER_ALLOWED - 5 + 1)) - 1];

static uint32_t  usedBytes = 0, freeBytes = 0;

static inline uint32_t order_size(uint32_t order) { return 1u << order; }

static inline uint32_t round_down_pow2(uint32_t x) {
    /* largest power-of-two ≤ x (x>0) */
    x |= (x >> 1); x |= (x >> 2); x |= (x >> 4);
    x |= (x >> 8); x |= (x >> 16);
    return x - (x >> 1);
}

static uint32_t idx_to_offset(uint32_t idx, uint32_t order) {
    uint32_t depth = maxOrder - order;
    uint32_t firstIdxAtDepth = (1u << depth) - 1;
    uint32_t posInLevel      = idx - firstIdxAtDepth;
    return posInLevel * order_size(order);
}

void createMemoryManager1(void *memoryStart, uint32_t memoryBytes) {
    if (!memoryStart || memoryBytes < MIN_BLOCK_SIZE) return;

    poolBase = (uint8_t *)memoryStart;
    poolSize = round_down_pow2(memoryBytes);
    if (poolSize > (1u << MAX_ORDER_ALLOWED))
        poolSize = (1u << MAX_ORDER_ALLOWED);

    maxOrder = 0;
    while (order_size(maxOrder) < poolSize) maxOrder++;

    uint32_t treeNodes = (1u << (maxOrder - 5 + 1)) - 1;
    for (uint32_t i = 0; i < treeNodes; i++) nodeState[i] = FREE;

    usedBytes = 0;
    freeBytes = poolSize;
}

static uint32_t size_to_order(uint32_t userSize) {
    uint32_t need = ALIGN(userSize) + HDR_SIZE;
    uint32_t ord = 0;
    while (order_size(ord) < need) ord++;
    /* never below MIN_BLOCK_SIZE */
    if (order_size(ord) < MIN_BLOCK_SIZE) ord = 5;   
    return ord;
}

static int alloc_rec(uint32_t idx, uint32_t order, uint32_t wantOrder, uint32_t *outOffset)
{
    if (nodeState[idx] == FULL) return 0;

    if (order == wantOrder) {
        if (nodeState[idx] == FREE) {
            nodeState[idx] = FULL;
            *outOffset = idx_to_offset(idx, order);
            return 1;
        }
        return 0;                             
    }

    if (nodeState[idx] == FREE)
        nodeState[idx] = SPLIT;


    uint32_t leftIdx  = 2 * idx + 1;
    uint32_t rightIdx = leftIdx + 1;
    uint32_t childOrd = order - 1;

    if (alloc_rec(leftIdx, childOrd, wantOrder, outOffset)) return 1;
    if (alloc_rec(rightIdx, childOrd, wantOrder, outOffset)) return 1;

    if (nodeState[leftIdx] == FREE && nodeState[rightIdx] == FREE)
        nodeState[idx] = FREE;

    return 0;
}

void *allocMemory1(uint32_t size) {
    if (!poolBase || size == 0) return NULL;

    uint32_t wantOrder = size_to_order(size);
    if (wantOrder > maxOrder) return NULL;

    uint32_t offset = 0;
    if (!alloc_rec(0, maxOrder, wantOrder, &offset)) {
        return NULL;                                  
    } 

    AllocHdr *hdr = (AllocHdr *)(poolBase + offset);
    hdr->order = (uint8_t)wantOrder;

    usedBytes += order_size(wantOrder) - HDR_SIZE;
    freeBytes -= order_size(wantOrder);

    return (void *)((uint8_t *)hdr + HDR_SIZE);
}

static void free_rec(uint32_t idx, uint32_t order, uint32_t blockOffset, uint32_t blockOrder) {
    if (order < blockOrder) return;               

    if (order == blockOrder) {
        nodeState[idx] = FREE;
    } else {
        uint32_t leftIdx  = 2 * idx + 1;
        uint32_t rightIdx = leftIdx + 1;
        uint32_t childOrd = order - 1;
        uint32_t halfSize = order_size(childOrd);

        if (blockOffset & halfSize)
            free_rec(rightIdx, childOrd, blockOffset - halfSize, blockOrder);
        else
            free_rec(leftIdx, childOrd, blockOffset, blockOrder);

        
        if (nodeState[leftIdx] == FREE && nodeState[rightIdx] == FREE)
            nodeState[idx] = FREE;
        else
            nodeState[idx] = SPLIT;
    }
}

void freeMemory1(void *ptr) {
    if (!ptr || !poolBase) return;

    AllocHdr *hdr = (AllocHdr *)((uint8_t *)ptr - HDR_SIZE);
    uint32_t order = hdr->order;
    uint32_t offset = (uint32_t)((uint8_t *)hdr - poolBase);

    free_rec(0, maxOrder, offset, order);

    usedBytes -= order_size(order) - HDR_SIZE;
    freeBytes += order_size(order);
}


void getMemoryStatus1(MemoryStatus *status) {
    if (!status) return;
    status->total = poolSize;
    status->used  = usedBytes;
    status->free  = freeBytes;
    status->base  = (void *)poolBase;
    status->end   = (void *)(poolBase + poolSize);
}

/* helpers para el test */
static uint32_t nextPow2(uint32_t v) {
    if (v == 0) return 1;
    v--; v |= v >> 1; v |= v >> 2; v |= v >> 4;
    v |= v >> 8; v |= v >> 16;
    return v + 1;
}

static void *createPool1(void) {
    void *pool = malloc(MANAGED_MEMORY_SIZE);
    CuAssertPtrNotNullMsg(NULL, "malloc failed in test", pool);
    createMemoryManager1(pool, MANAGED_MEMORY_SIZE);
    return pool;
}

/* ------------------------------------------------------------------ */
/*  PRUEBA: Estado inicial                                            */
static void testInitialization(CuTest *tc) {
    void *pool = createPool1();

    MemoryStatus st;
    getMemoryStatus1(&st);

    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE,  st.total);
    CuAssertIntEquals(tc, 0,                    st.used);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE,  st.free);   /* buddy: pool libre completa */
    CuAssertPtrEquals(tc, pool,                 st.base);
    CuAssertPtrEquals(tc, (uint8_t*)pool + MANAGED_MEMORY_SIZE, st.end);

    free(pool);
}

/* ------------------------------------------------------------------ */
/*  PRUEBA: Una sola reserva                                          */
static void testSingleAllocation(CuTest *tc) {
    void *pool = createPool1();

    void *ptr = allocMemory1(ALLOCATION_SIZE);
    CuAssertPtrNotNull(tc, ptr);
    CuAssertTrue(tc, ((uintptr_t)ptr % ALIGNMENT) == 0);

    /* cálculo del used esperado según algoritmo buddy */
    uint32_t need      = ALIGN(ALLOCATION_SIZE) + HDR_SIZE;
    uint32_t blkSize   = nextPow2(need);
    uint32_t expectedUsed = blkSize - HDR_SIZE;

    MemoryStatus st;
    getMemoryStatus1(&st);

    CuAssertIntEquals(tc, expectedUsed, st.used);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE - blkSize, st.free);
    CuAssertTrue(tc, ptr >= st.base && ptr < st.end);

    free(pool);
}

/* ------------------------------------------------------------------ */
/*  PRUEBA: liberar y coalescer                                        */
static void testFreeAndCoalesce(CuTest *tc) {
    void *pool = createPool1();

    void *p1 = allocMemory1(32);
    void *p2 = allocMemory1(48);
    CuAssertPtrNotNull(tc, p1);
    CuAssertPtrNotNull(tc, p2);

    freeMemory1(p1);
    freeMemory1(p2);

    MemoryStatus st;
    getMemoryStatus1(&st);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE, st.free);
    CuAssertIntEquals(tc, 0,                   st.used);

    free(pool);
}

/* ------------------------------------------------------------------ */
/*  PRUEBA: out-of-memory                                              */
static void testOutOfMemory(CuTest *tc) {
    void *pool = createPool1();

    /* Demasiado grande (necesitaría > pool por el header) */
    void *big = allocMemory1(MANAGED_MEMORY_SIZE);
    CuAssertPtrEquals(tc, NULL, big);

    /* Cabe justo: payload + header redondea a 2048 */
    void *full = allocMemory1(MANAGED_MEMORY_SIZE - 2*HDR_SIZE);
    CuAssertPtrNotNull(tc, full);

    /* pool saturado -> debe fallar */
    void *fail = allocMemory1(ALIGNMENT);
    CuAssertPtrEquals(tc, NULL, fail);

    free(pool);
}

/* ------------------------------------------------------------------ */
/*  PRUEBA: varias reservaciones seguidas                              */
static void testMultipleAllocations(CuTest *tc) {
    void *pool = createPool1();

    void *p[4];
    for (int i = 0; i < 4; i++)
        p[i] = allocMemory1(ALLOCATION_SIZE);

    for (int i = 0; i < 4; i++)
        CuAssertPtrNotNull(tc, p[i]);

    /* Verificar que cada bloque está alineado y no se superpone        */
    uint32_t blkSz = nextPow2(ALIGN(ALLOCATION_SIZE) + HDR_SIZE);
    for (int i = 1; i < 4; i++) {
        uint8_t *prevEnd = (uint8_t*)p[i-1] - HDR_SIZE + blkSz;
        CuAssertTrue(tc, prevEnd <= (uint8_t*)p[i]);
    }

    for (int i = 3; i >= 0; i--)
        freeMemory1(p[i]);

    MemoryStatus st;
    getMemoryStatus1(&st);
    CuAssertIntEquals(tc, MANAGED_MEMORY_SIZE, st.free);
    CuAssertIntEquals(tc, 0,                   st.used);

    free(pool);
}

/* ------------------------------------------------------------------ */
/*  Registro de la suite para CuTest                                   */
CuSuite *getBuddyMemoryManagerTestSuite(void) {
    CuSuite *suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testInitialization);
    SUITE_ADD_TEST(suite, testSingleAllocation);
    SUITE_ADD_TEST(suite, testFreeAndCoalesce);
    SUITE_ADD_TEST(suite, testOutOfMemory);
    SUITE_ADD_TEST(suite, testMultipleAllocations);
    return suite;
}
