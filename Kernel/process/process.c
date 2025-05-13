#include "process.h"
#include "memoryManager.h"

static processControlBlockADT newPcb(processManagerADT pm,
                                     pid_t parent,
                                     uint64_t (*start)(char **, int),
                                     char **argv) {
    processControlBlockADT pcb =
        allocMemory(sizeof(struct processControlBlockCDT));
    if (!pcb) return NULL;

    pcb->pid       = pm->nextPid++;
    pcb->parentPid = parent;
    pcb->startFunc = start;
    pcb->argv      = argv;

    pcb->priority  = PRIORITY_MEDIUM;
    pcb->tickets   = PRIORITY_MEDIUM;

    pcb->stackTop  = NULL;
    pcb->rsp       = 0;
    pcb->state     = STATE_NEW;

    return pcb;
}

static processControlBlockADT findPcb(processManagerADT pm, pid_t pid) {
    return (pid < MAX_PROCESSES) ? pm->table[pid] : NULL;
}

processManagerADT initProcessManager(schedulerADT sch) {
    processManagerADT pm = allocMemory(sizeof(struct processManagerCDT));
    if (!pm) return NULL;

    pm->scheduler = sch;
    for (int i = 0; i < MAX_PROCESSES; i++) pm->table[i] = NULL;
    pm->nextPid   = 1;
    pm->numProcs  = 0;
    return pm;
}

pid_t createProcess(processManagerADT pm, pid_t parent,
                    uint64_t (*start)(char **, int), char **argv) {
    if (pm->numProcs >= MAX_PROCESSES) return -1;

    processControlBlockADT pcb = newPcb(pm, parent, start, argv);
    if (!pcb) return -1;

    pm->table[pcb->pid] = pcb;
    pm->numProcs++;

    pcb->state = STATE_READY;
    scheduleProcess(pm->scheduler, pcb);

    return pcb->pid;
}

int exitProcess(processManagerADT pm, pid_t pid) {
    processControlBlockADT pcb = findPcb(pm, pid);
    if (!pcb) return -1;

    pcb->state = STATE_ZOMBIE;
    descheduleProcess(pm->scheduler, pcb);
    return 0;
}

int blockProcess(processManagerADT pm, pid_t pid) {
    processControlBlockADT pcb = findPcb(pm, pid);
    if (!pcb) return -1;

    pcb->state = STATE_BLOCKED;
    descheduleProcess(pm->scheduler, pcb);
    return 0;
}

int unblockProcess(processManagerADT pm, pid_t pid) {
    processControlBlockADT pcb = findPcb(pm, pid);
    if (!pcb) return -1;

    pcb->state = STATE_READY;
    scheduleProcess(pm->scheduler, pcb);
    return 0;
}

int killProcess(processManagerADT pm, pid_t pid, uint64_t recursive) {
    processControlBlockADT pcb = findPcb(pm, pid);
    if (!pcb) return -1;

    pcb->state = STATE_KILLED;
    descheduleProcess(pm->scheduler, pcb);

    pm->table[pid] = NULL;
    freeMemory(pcb);
    pm->numProcs--;

    if (recursive) {
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (pm->table[i] && pm->table[i]->parentPid == pid) {
                killProcess(pm, i, 1);
            }
        }
    }
    return 0;
}

processControlBlockADT getProcess(processManagerADT pm, pid_t pid) {
    return findPcb(pm, pid);
}

uint64_t getNumProcesses(processManagerADT pm) {
    return pm->numProcs;
}
