#ifndef __PROCESS_STRUCTURE_H
#define __PROCESS_STRUCTURE_H

#include <stdint.h>

#define MAX_NAME_LENGTH 20
typedef enum
{
    READY = 0,
    RUNNING,
    BLOCKED,
    DEAD,
} ProcessState;

typedef unsigned int Priority;
typedef long PID;
typedef int (*entryPoint)(int argc, char *argv[]);

typedef struct
{
    int argc;
    char **argv;
    char *name;
    Priority priority;
    entryPoint entryPoint;
    int foreground;
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
    int returnValue;
} Process;

#endif