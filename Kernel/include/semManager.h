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

typedef int sem_t;

typedef struct semBlock {
    int value;
    char used;
    int locked; // For spinlock
    queueADT waiting;
    char openedBy[MAX_PROCESSES];
    char name[SEM_MAX_NAME_LENGTH];
} semBlock;

int initSemManager();

// Creates and open for current process
sem_t semCreate(const char * name, int value);

int semExists(int semId);

sem_t findSemByName(const char * name);

sem_t semOpen(const char * name);

// Close but never destroy
int semClose(sem_t semId);

int semWait(sem_t semId);

int semPost(sem_t semId);

int semDestroy(sem_t semId);

#endif