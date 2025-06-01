#ifndef __QUEUEADT_H
#define __QUEUEADT_H

#include <stdint.h>
#include "../include/memoryManager.h"

typedef long type;

typedef struct queueCDT * queueADT;

queueADT newQueue();

void queue(queueADT q, type element);

type dequeue(queueADT q);

uint32_t isEmpty(queueADT q);

void freeQueue(queueADT q);

#endif