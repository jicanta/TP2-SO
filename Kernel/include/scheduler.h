#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <defs.h>
#include "queueADT.h"
#include "PCB.h"
#include <stdint.h>
#include <stddef.h>

typedef struct schedulerCDT {
    queueADT               readyQueue;
    processControlBlockADT current;
} *schedulerADT;

schedulerADT initScheduler(void);

int      scheduleProcess  (schedulerADT sch, processControlBlockADT pcb);
int      descheduleProcess(schedulerADT sch, processControlBlockADT pcb);

pid_t    getCurrentPid    (schedulerADT sch);
uint64_t contextSwitch    (schedulerADT sch, uint64_t rsp);

#endif
