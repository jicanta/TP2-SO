#ifndef _QUEUE_ADT_H_
#define _QUEUE_ADT_H_

#include <stdint.h>

typedef struct queueNode {
    void *data;
    struct queueNode *next;
} queueNode;

typedef struct queueCDT {
    queueNode *head;
    queueNode *tail;
    uint64_t size;
} *queueADT;

queueADT createQueue(void);
void destroyQueue(queueADT q);

void enqueue(queueADT q, void *elem);
void *dequeue(queueADT q);
void *dequeueValue(queueADT q, void *elem);

int queueIsEmpty(queueADT q);
uint64_t queueSize(queueADT q);
uint64_t countOccurrences(queueADT q, void *elem);

#endif
