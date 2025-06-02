#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "memoryManager.h"
#include "queueADT.h"
#include "process.h"
#include "scheduler.h"
#include "interrupts.h" 

#define MAX_SEMS 1000

typedef long PID;

typedef struct sem_t {
    int value;
    int used;
    queueADT waiting;
    PID openedBy[MAX_PROCESSES];
} sem_t;

int initSemManager();

// Creates and open for current process
int semCreate(int value);

int semExists(int semId);

int semOpen(int semId);

// Close but never destroy
int semClose(int semId);

int semWait(int semId);

int semPost(int semId);

int semDestroy(int semId);

#endif