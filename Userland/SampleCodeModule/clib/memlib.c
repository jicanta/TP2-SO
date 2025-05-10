#include <memlib.h>
#include <syscalls.h>

void *malloc(uint32_t size){
    void *ptr = _sys_malloc(size);
    if(ptr == NULL){
        return NULL;
    }

    return ptr;
}

void free(void * memorySegment){
    _sys_free(memorySegment);
    printf("Memory freed successfully.\n");
    return;
}