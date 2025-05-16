#include "scheduler.h"
#include "memoryManager.h"

static uint32_t rng32(void) {
    static uint32_t mz = 362436069;
    static uint32_t mw = 521288629;
    mz = 36969 * (mz & 0xFFFF) + (mz >> 16);
    mw = 18000 * (mw & 0xFFFF) + (mw >> 16);
    return (mz << 16) + mw;
}

schedulerADT initScheduler(void) {
    schedulerADT s = allocMemory(sizeof(struct schedulerCDT));
    if (!s) return NULL;
    s->readyQueue = createQueue();
    s->current    = NULL;
    return s;
}

int scheduleProcess(schedulerADT s, PCB * pcb) {
    if (!s || !pcb) return -1;
    pcb->state = READY;
    enqueue(s->readyQueue, pcb);
    return 0;
}

int descheduleProcess(schedulerADT s, PCB * pcb) {
    if (!s || !pcb) return -1;
    dequeueValue(s->readyQueue, pcb);
    return 0;
}

pid_t getCurrentPid(schedulerADT s) {
    return (s && s->current) ? s->current->pid : -1;
}

uint64_t * contextSwitch(schedulerADT s, uint64_t * rsp) {
    if (s->current) s->current->stackBase = rsp;

    if (queueIsEmpty(s->readyQueue)) {
        if (s->current) {
            s->current->state = RUNNING;
            return s->current->stackBase;
        }
        return rsp;
    }

    uint32_t totalTickets = 0;
    uint64_t   readyCount   = queueSize(s->readyQueue);

    for (uint64_t i = 0; i < readyCount; i++) {
        PCB * pcb = dequeue(s->readyQueue);
        enqueue(s->readyQueue, pcb);
        totalTickets += pcb->priority;
    }

    uint32_t draw = 1 + rng32() % totalTickets;
    PCB * winner = NULL;

    while (1) {
        PCB * pcb = dequeue(s->readyQueue);
        if (draw <= pcb->priority) {
            winner = pcb;
            break;
        }
        draw -= pcb->priority;
        enqueue(s->readyQueue, pcb);
    }

    if (s->current && s->current->state == RUNNING) {
        s->current->state = READY;
        enqueue(s->readyQueue, s->current);
    }

    winner->state = RUNNING;
    s->current    = winner;
    return winner->stackBase;
}