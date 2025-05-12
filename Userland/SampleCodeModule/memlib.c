#include "include/memlib.h"

void * malloc(uint32_t size){
    return sysMalloc(size);
}

void free(void * memorySegment){
    sysFree(memorySegment);
    return;
}