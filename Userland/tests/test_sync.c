// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include "../SampleCodeModule/include/lib.h"
#include "test_util.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 3

int32_t global; // shared memory

void slowInc(int32_t *p, int32_t inc) {
  uint32_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint32_t my_process_inc(uint32_t argc, char *argv[]) {
  uint32_t n;
  int8_t inc;
  int8_t use_sem;
  int semId;

  if (argc != 3){
    return -1;
  }
  if ((n = satoi(argv[0])) <= 0){
    return -1;
  }
  if ((inc = satoi(argv[1])) == 0){
    return -1;
  }
  if ((use_sem = satoi(argv[2])) < 0){
    return -1;
  }

  if (use_sem)
    if ((semId = semOpen("/test_sync_1", 1)) < 0) {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint32_t i;
  for (i = 0; i < n; i++) {
    if (use_sem){
      semWait(semId);
    }
    slowInc(&global, inc);
    if (use_sem){
      semPost(semId);
    }
  }

  if (use_sem)
    semClose(semId);

  return 0;
}

uint32_t test_sync(uint32_t argc, char *argv[]) { //{n, use_sem, 0}
  uint32_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2){
    printf("test_sync: ERROR missing arguments\n");  
    return -1;
  }

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint32_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    creationParameters params;
    params.argc = 3;
    params.argv = argvDec;
    params.name = "my_process_inc";
    params.priority = 1;
    params.entryPoint = (entryPoint)my_process_inc;
    params.foreground = 0;
    params.fds[0] = STDIN;
    params.fds[1] = STDOUT;
    pids[i] = createProcess(&params);
    params.argv = argvInc;
    pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&params);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    waitProcess(pids[i], NULL);
    waitProcess(pids[i + TOTAL_PAIR_PROCESSES], NULL);
  }

  int semId = semOpen("/test_sync_1", 1);
  semDestroy(semId);

  printf("Final value: %d\n", global);

  return 0;
}
