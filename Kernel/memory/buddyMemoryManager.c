// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/memoryManager.h"
#include <stdint.h>

typedef struct {
    uint8_t  order;     
    uint8_t  _pad[7];    
} AllocHdr;

#define HDR_SIZE       ((uint32_t)sizeof(AllocHdr))

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

void createMemoryManager(void *memoryStart, uint32_t memoryBytes) {
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

void *allocMemory(uint32_t size) {
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

void freeMemory(void *ptr) {
    if (!ptr || !poolBase) return;

    AllocHdr *hdr = (AllocHdr *)((uint8_t *)ptr - HDR_SIZE);
    uint32_t order = hdr->order;
    uint32_t offset = (uint32_t)((uint8_t *)hdr - poolBase);

    free_rec(0, maxOrder, offset, order);

    usedBytes -= order_size(order) - HDR_SIZE;
    freeBytes += order_size(order);
}


void getMemoryStatus(MemoryStatus *status) {
    if (!status) return;
    status->total = poolSize;
    status->used  = usedBytes;
    status->free  = freeBytes;
    status->base  = (void *)poolBase;
    status->end   = (void *)(poolBase + poolSize);
}