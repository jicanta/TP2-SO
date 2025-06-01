#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include <stdint.h>
#include "../include/memoryManager.h"
#include "../include/queueADT.h"
#include "../include/process.h"
#include "../include/scheduler.h"

#define MAX_SEMS 1000

typedef long PID;

typedef struct sem_t {
    uint32_t value;
    uint32_t used;
    queueADT waiting;
} sem_t;

uint32_t initSemManager();

#endif