#ifndef __MEMORY_STRUCTURE_H_
#define __MEMORY_STRUCTURE_H_

#include "stdint.h"

typedef struct {
    uint32_t total;  
    uint32_t used;   
    uint32_t free;    
    void    *base;    
    void    *end;     
} MemoryStatus;

#endif