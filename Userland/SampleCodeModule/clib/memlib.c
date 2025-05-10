#include <memlib.h>
#include <syscalls.h>

void *malloc(uint32_t size){
    void *ptr = dispatcherMalloc(size);
    if(ptr == NULL){
        return NULL;
    }

    return ptr;
}

void free(void * memorySegment){
    dispatcherFree(memorySegment);
    printf("Memory freed successfully.\n");
    return;
}