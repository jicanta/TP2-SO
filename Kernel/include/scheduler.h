#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#define YIELD_DONE 1
#define YIELD_NOT_DONE 0
#include "./process.h"
#include <stdint.h>
typedef struct Node
{
    Process * pcb;
    struct Node * next;
    uint32_t executionsLeft;
}Node;

typedef struct List
{
    Node * head ;
    Node * tail ;
}List;


/*
 * Initializes the scheduler for the initial process.
 */
void initInitScheduler();

/*
 * Initializes the scheduler system.
 */
void initScheduler();

/*
 * Adds a process to the scheduler queue.
 * Parameters:
 *   pcb - Pointer to the process control block of the process to schedule.
 */
void schedule(Process * pcb);

/*
 * Removes and retrieves the next process from the scheduler queue.
 * Returns a pointer to the process control block of the unscheduled process.
 */
Process * unschedule();

/*
 * Switches the CPU context to the next process.
 * Parameters:
 *   rsp - Pointer to the current stack pointer.
 * Returns the stack pointer of the next process.
 */
uint64_t * switchContent(uint64_t * rsp);

/*
 * Retrieves the process currently being executed.
 * Returns a pointer to the process control block of the current process.
 */
Process * getCurrentProcess();

/*
 * Blocks a process by its PID.
 * Parameters:
 *   pid - The PID of the process to block.
 * Returns 0 on success, or a negative value on failure.
 */
int blockProcess(PID pid);

/*
 * Unblocks a process by its PID.
 * Parameters:
 *   pid - The PID of the process to unblock.
 * Returns 0 on success, or a negative value on failure.
 */
int unblockProcess(PID pid);

/*
 * Sets the yield flag to indicate that the current process should yield.
 */
void setYield();

/*
 * Clears the yield flag.
 */
void clearYield();

/*
 * Retrieves the current state of the yield flag.
 * Returns 1 if the yield flag is set, or 0 otherwise.
 */
char getYield();

/*
 * Performs garbage collection to clean up unused resources.
 */
void garbageCollect();
#endif