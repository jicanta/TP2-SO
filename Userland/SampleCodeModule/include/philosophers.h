#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#include "stdio.h"
#include "string.h"
#include "processStructure.h"
#include "lib.h"
#include "syscalls.h"
#include "../../tests/test_util.h"

#define STARTING_PHILOSOPHERS 2
#define MAX_PHILOSOPHERS 16

#define EATING 0
#define THINKING 1

typedef struct philosopher {
    PID pid;
    char name[MAX_NAME_LENGTH];
} phi_t;

uint64_t startPhilo(uint64_t argc, char *argv[]);

void philosopher(uint64_t argc, char *argv[]);


#endif