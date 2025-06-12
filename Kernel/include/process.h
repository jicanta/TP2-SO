#ifndef __PROCESS_H
#define __PROCESS_H

#define MAX_NAME_LENGTH 20
#define MAX_PROCESSES 100
#define MAX_PRIORITY 5
#define MIN_PRIORITY 1
#define DEFAULT_PRIORITY MIN_PRIORITY
#define MAX_PID MAX_PROCESSES
#define NONPID -1
#define INITPID 1

#include "defs.h"
#include <stdint.h>
typedef int (*entryPoint)(int argc, char *argv[]);

typedef struct
{
    int argc;
    char **argv;
    char *name;
    Priority priority;
    entryPoint entryPoint;
    int foreground;
    int fds[2];
} creationParameters;

typedef struct
{
    char name[MAX_NAME_LENGTH];
    PID pid, parentpid, waitingPID;
    int argc;
    char **argv;
    Priority priority;
    entryPoint entryPoint;
    int foreground;
    ProcessState state;
    uint64_t *stackBase, *stackEnd;
    int childReturnValue;
    int fds[2]; // File descriptors
} Process;

/*
 * Retrieves the process currently in the foreground of the terminal.
 * Returns a pointer to the foreground process.
 */
Process * getTerminalForegroundProcess(void);

/*
 * Initializes the process management system.
 * Returns the PID of the initial process.
 */
PID initProcesses(void);

/*
 * Creates a new process with the specified parameters.
 * Parameters:
 *   params - Pointer to a creationParameters structure containing process details.
 * Returns the PID of the created process, or a negative value on failure.
 */
PID createProcess(creationParameters *params);

/*
 * Sets up the stack for a new process.
 * Parameters:
 *   argc - Number of arguments.
 *   argv - Array of argument strings.
 *   entry - Entry point of the process.
 *   stackBase - Base address of the stack.
 *   processLoader - Function to load the process.
 * Returns a pointer to the top of the stack.
 */
uint64_t *setupStack(int argc, char **argv, entryPoint entry, uint64_t *stackBase, entryPoint processLoader);

/*
 * Retrieves the PID of the current process.
 * Returns the PID of the current process.
 */
PID getpid(void);

/*
 * Retrieves the PID of the parent process.
 * Returns the PID of the parent process.
 */
PID getppid(void);

/*
 * Retrieves information about a process by its PID.
 * Parameters:
 *   pid - The PID of the process to retrieve.
 * Returns a pointer to the Process structure, or NULL if the PID is invalid.
 */
Process *getProcess(PID pid);

/*
 * Terminates a process by its PID.
 * Parameters:
 *   pid - The PID of the process to terminate.
 * Returns 0 on success, or a negative value on failure.
 */
int kill(PID pid);

/*
 * Terminates all child processes of a given PID.
 * Parameters:
 *   pid - The PID of the parent process.
 * Returns the number of terminated child processes, or a negative value on failure.
 */
int killAllChildren(PID pid);

/*
 * Waits for a process to finish execution.
 * Parameters:
 *   pidToWait - The PID of the process to wait for.
 *   wstatus - Pointer to store the exit status of the process.
 */
void waitProcess(PID pidToWait, int *wstatus);

/*
 * Retrieves information about all processes.
 * Returns a pointer to an array of Process structures.
 */
Process *getProcessesInformation();

/*
 * Frees the memory allocated for process information.
 * Parameters:
 *   processesInfo - Pointer to the array of Process structures to free.
 */
void freeProcessesInformation(Process *processesInfo);

/*
 * Changes the priority of a process.
 * Parameters:
 *   pid - The PID of the process.
 *   priority - The new priority value.
 * Returns 0 on success, or a negative value on failure.
 */
int changeProccessPriority(PID pid, Priority priority);

/*
 * Checks if a PID is valid.
 * Parameters:
 *   pid - The PID to check.
 * Returns 1 if the PID is valid, or 0 otherwise.
 */
int isValidPID(PID pid);

/*
 * Validates a priority value.
 * Parameters:
 *   priority - The priority value to check.
 * Returns 1 if the priority is valid, or 0 otherwise.
 */
int checkPriority(Priority priority);

/*
 * Retrieves the file descriptors associated with a process.
 * Parameters:
 *   fds - Pointer to an array to store the file descriptors.
 * Returns the number of file descriptors, or a negative value on failure.
 */
int getFileDescriptors(int *fds);

/*
 * Blocks all processes except the shell.
 */
void blockAllExceptShell();
#endif