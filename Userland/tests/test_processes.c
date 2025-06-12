// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include "test_util.h"
#include "../SampleCodeModule/include/lib.h"

#define ITERATIONS 1000

typedef struct P_rq {
  int32_t pid;
  ProcessState state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {

  printf("Running process manager test...\n");

  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1){
    printf("test_processes: ERROR argument missing\n");
    return -1;
  }

  if ((max_processes = satoi(argv[0])) <= 0){
    printf("test_processes: ERROR parsing max processes\n");
    return -1;
  }

  p_rq p_rqs[max_processes];

  int iterations = ITERATIONS;

  while (iterations--) {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      creationParameters params;
      params.argc = 0;
      params.argv = argvAux;
      params.name = "endless_loop_print";
      params.priority = 1;
      params.entryPoint = (entryPoint)endless_loop_print;
      params.foreground = 0;
      params.fds[0] = STDIN;
      params.fds[1] = STDOUT;
      p_rqs[rq].pid = createProcess(&params);

      if (p_rqs[rq].pid == -1) {
        printf("test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (kill(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR killing process\n");
                return -1;
              }
              p_rqs[rq].state = DEAD;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (block(p_rqs[rq].pid) == -1) {
                printf("test_processes: ERROR blocking process\n");
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (unblock(p_rqs[rq].pid) == -1) {
            printf("test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }

  printf("OK!\n");
  printf("Execution finished succesfully\n");
  return 0;
}
