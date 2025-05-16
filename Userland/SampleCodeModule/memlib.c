#include "include/memlib.h"
#include "include/syscalls.h"

void * malloc(uint32_t size){
    return sysMalloc(size);
}

void free(void * memorySegment){
    sysFree(memorySegment);
    return;
}