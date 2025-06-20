// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include "../SampleCodeModule/include/stdio.h"
#include "../SampleCodeModule/include/string.h"
#include "../SampleCodeModule/include/lib.h"
#include "test.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  printf("Running memory manager test.\n");
  printf("\n");
  printf("If an error occurs, the execution will be cut off and you will be informed.\n");
  printf("Otherwise, nothing will happen.\n");

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;


  if (argc != 1){
    printf("test_mm: invalid arguments\n");
    return -1;
  }

  if ((max_memory = satoi(argv[0])) <= 0){
    printf("test_mm: invalid max_memory\n");
    return -1;
  }

  
  while(1){

    rq = 0;
    total = 0;
  
    
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

      
    // Set
      uint32_t i;
      for (i = 0; i < rq; i++)
        if (mm_rqs[i].address)
          memset(mm_rqs[i].address, i, mm_rqs[i].size);

      // Check
      for (i = 0; i < rq; i++)
        if (mm_rqs[i].address)
          if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
            printf("test_mm ERROR\n");
            return -1;
          }

      // Free
      for (i = 0; i < rq; i++)
        if (mm_rqs[i].address)
          free(mm_rqs[i].address);

  }

  return 0;
}
