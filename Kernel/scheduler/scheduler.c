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

int scheduleProcess(schedulerADT s, processControlBlockADT pcb) {
    if (!s || !pcb) return -1;
    pcb->state = STATE_READY;
    enqueue(s->readyQueue, pcb);
    return 0;
}

int descheduleProcess(schedulerADT s, processControlBlockADT pcb) {
    if (!s || !pcb) return -1;
    dequeueValue(s->readyQueue, pcb);
    return 0;
}

pid_t getCurrentPid(schedulerADT s) {
    return (s && s->current) ? s->current->pid : -1;
}

uint64_t contextSwitch(schedulerADT s, uint64_t rsp) {
    if (s->current) s->current->rsp = rsp;

    if (queueIsEmpty(s->readyQueue)) {
        if (s->current) {
            s->current->state = STATE_RUNNING;
            return s->current->rsp;
        }
        return rsp;
    }

    uint32_t totalTickets = 0;
    size_t   readyCount   = queueSize(s->readyQueue);

    for (size_t i = 0; i < readyCount; i++) {
        processControlBlockADT pcb = dequeue(s->readyQueue);
        enqueue(s->readyQueue, pcb);
        totalTickets += pcb->tickets;
    }

    uint32_t draw = 1 + rng32() % totalTickets;
    processControlBlockADT winner = NULL;

    while (1) {
        processControlBlockADT pcb = dequeue(s->readyQueue);
        if (draw <= pcb->tickets) {
            winner = pcb;
            break;
        }
        draw -= pcb->tickets;
        enqueue(s->readyQueue, pcb);
    }

    if (s->current && s->current->state == STATE_RUNNING) {
        s->current->state = STATE_READY;
        enqueue(s->readyQueue, s->current);
    }

    winner->state = STATE_RUNNING;
    s->current    = winner;
    return winner->rsp;
}