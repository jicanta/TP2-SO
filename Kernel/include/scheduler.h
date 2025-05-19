#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#define YIELD_DONE 1
#define YIELD_NOT_DONE 0
#include "./process.h"
#include <stdint.h>
typedef struct Node
{
    Process * pcb;
    struct Node * next;
    uint32_t executionsLeft;
}Node;

typedef struct List
{
    Node * head ;
    Node * tail ;
}List;


void initInitScheduler();
void initScheduler();

void schedule(Process * pcb);
Process * unschedule();
uint64_t * switchContent(uint64_t * rsp);

Process * getCurrentProcess();
int blockProcess(PID pid);
int unblockProcess(PID pid);

void setYield();
void clearYield();
char getYield();
void garbageCollect();
#endif