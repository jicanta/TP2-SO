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

Process * getTerminalForegroundProcess(void);
PID initProcesses(void);
PID createProcess(creationParameters *params);
uint64_t *setupStack(int argc, char **argv, entryPoint entry, uint64_t *stackBase, entryPoint processLoader);
PID getpid(void);
PID getppid(void);
Process *getProcess(PID pid);
int kill(PID pid);
void waitProcess(PID pidToWait, int *wstatus);
Process *getProcessesInformation();
void freeProcessesInformation(Process *processesInfo);
int changeProccessPriority(PID pid, Priority priority);
int isValidPID(PID pid);
int checkPriority(Priority priority);
int getFileDescriptors(int *fds);
#endif