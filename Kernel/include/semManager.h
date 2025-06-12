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

/*
 * Initializes the semaphore manager.
 * Returns 0 on success, or a negative value on failure.
 */
int initSemManager();

/*
 * Creates a new semaphore and opens it for the current process.
 * Parameters:
 *   name - The name of the semaphore.
 *   value - The initial value of the semaphore.
 * Returns the ID of the created semaphore, or a negative value on failure.
 */
int semCreate(const char * name, int value);

/*
 * Checks if a semaphore exists by its ID.
 * Parameters:
 *   semId - The ID of the semaphore.
 * Returns 1 if the semaphore exists, or 0 otherwise.
 */
int semExists(int semId);

/*
 * Finds a semaphore by its name.
 * Parameters:
 *   name - The name of the semaphore.
 * Returns the ID of the semaphore, or a negative value if not found.
 */
int findSemByName(const char * name);

/*
 * Opens an existing semaphore for the current process.
 * Parameters:
 *   name - The name of the semaphore.
 * Returns the ID of the opened semaphore, or a negative value on failure.
 */
int semOpen(const char * name);

/*
 * Closes a semaphore for the current process without destroying it.
 * Parameters:
 *   semId - The ID of the semaphore to close.
 * Returns 0 on success, or a negative value on failure.
 */
int semClose(int semId);

/*
 * Decrements the value of a semaphore, blocking the process if the value is zero.
 * Parameters:
 *   semId - The ID of the semaphore.
 * Returns 0 on success, or a negative value on failure.
 */
int semWait(int semId);

/*
 * Increments the value of a semaphore, unblocking any waiting processes.
 * Parameters:
 *   semId - The ID of the semaphore.
 * Returns 0 on success, or a negative value on failure.
 */
int semPost(int semId);

/*
 * Destroys a semaphore, freeing its resources.
 * Parameters:
 *   semId - The ID of the semaphore to destroy.
 * Returns 0 on success, or a negative value on failure.
 */
int semDestroy(int semId);

/*
 * Retrieves the current value of a semaphore.
 * Parameters:
 *   semId - The ID of the semaphore.
 * Returns the value of the semaphore, or a negative value on failure.
 */
int semValue(int semId);


#endif