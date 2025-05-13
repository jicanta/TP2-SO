#ifndef _PROCESS_MANAGER_H_
#define _PROCESS_MANAGER_H_

#include <defs.h>
#include <stddef.h>
#include "scheduler.h"
#include "PCB.h"
#include <stdint.h>

#define MAX_PROCESSES 1024

typedef struct processManagerCDT {
    schedulerADT            scheduler;
    processControlBlockADT  table[MAX_PROCESSES];
    pid_t                   nextPid;
    uint64_t                numProcs;
} *processManagerADT;

processManagerADT initProcessManager(schedulerADT sch);

pid_t createProcess(processManagerADT pm, pid_t parent,
                    uint64_t (*start)(char **, int), char **argv);

int exitProcess   (processManagerADT pm, pid_t pid);
int blockProcess  (processManagerADT pm, pid_t pid);
int unblockProcess(processManagerADT pm, pid_t pid);
int killProcess   (processManagerADT pm, pid_t pid, uint64_t recursive);

processControlBlockADT getProcess(processManagerADT pm, pid_t pid);
uint64_t               getNumProcesses(processManagerADT pm);

#endif
