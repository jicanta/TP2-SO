#include <stdint.h>

typedef struct {
    uint32_t total;  
    uint32_t used;   
    uint32_t free;    
    void    *base;    
    void    *end;     
} MemoryStatus;

void createMemoryManager(void *memoryStartAddress, uint32_t memorySize);

void *allocMemory(uint32_t size);

void freeMemory(void *memorySegment);

void getMemoryStatus(MemoryStatus *status);