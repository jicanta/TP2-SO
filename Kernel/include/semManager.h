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
    PID openedBy[MAX_PROCESSES];
} sem_t;

uint32_t initSemManager();

// Creates and open for current process
uint32_t semCreate(uint32_t value);

uint32_t semOpen(uint32_t semId);

// Close but never destroy
uint32_t semClose(uint32_t semId);

uint32_t semWait(uint32_t semId);

uint32_t semPost(uint32_t semId);

uint32_t semDestroy(uint32_t semId);

#endif