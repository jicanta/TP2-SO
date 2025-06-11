#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "memoryManager.h"
#include "queueADT.h"
#include "process.h"
#include "scheduler.h"
#include "interrupts.h" 
#include "lib.h"

#define MAX_SEMS 1000
#define SEM_MAX_NAME_LENGTH 32

typedef long PID;

typedef struct sem_t {
    int value;
    char used;
    int locked; // For spinlock
    queueADT waiting;
    char openedBy[MAX_PROCESSES];
    char name[SEM_MAX_NAME_LENGTH];
} sem_t;

int initSemManager();

// Creates and open for current process
int semCreate(const char * name, int value);

int semExists(int semId);

int findSemByName(const char * name);

int semOpen(const char * name);

// Close but never destroy
int semClose(int semId);

int semWait(int semId);

int semPost(int semId);

int semDestroy(int semId);

int semValue(int semId);


#endif