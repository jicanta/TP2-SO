#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <defs.h>
#include "queueADT.h"
#include <defs.h>
#include <stdint.h>

typedef struct schedulerCDT {
    queueADT               readyQueue;
    PCB *                  current;
} *schedulerADT;

schedulerADT initScheduler(void);

int      scheduleProcess  (schedulerADT sch, PCB * pcb);
int      descheduleProcess(schedulerADT sch, PCB * pcb);

pid_t    getCurrentPid    (schedulerADT sch);
uint64_t * contextSwitch    (schedulerADT sch, uint64_t * rsp);

#endif
