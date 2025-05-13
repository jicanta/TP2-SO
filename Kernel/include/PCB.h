#ifndef _PCB_H_
#define _PCB_H_

#include <defs.h>
#include <stddef.h>

typedef enum {
    PRIORITY_LOW    = 1,
    PRIORITY_MEDIUM = 5,
    PRIORITY_HIGH   = 10
} processPriority;

typedef enum {
    STATE_NEW,
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_ZOMBIE,
    STATE_KILLED
} processState;

typedef struct processControlBlockCDT {
    pid_t pid;
    pid_t parentPid;

    uint64_t (*startFunc)(char **, int);
    char **argv;

    processPriority priority;
    uint32_t tickets;

    uint64_t *stackTop;
    uint64_t rsp;

    processState state;
} *processControlBlockADT;

#endif
