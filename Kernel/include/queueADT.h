#ifndef _QUEUE_ADT_H_
#define _QUEUE_ADT_H_

#include <stddef.h>

typedef struct queueNode {
    void *data;
    struct queueNode *next;
} queueNode;

typedef struct queueCDT {
    queueNode *head;
    queueNode *tail;
    size_t size;
} *queueADT;

queueADT createQueue(void);
void destroyQueue(queueADT q);

void enqueue(queueADT q, void *elem);
void *dequeue(queueADT q);
void *dequeueValue(queueADT q, void *elem);

int queueIsEmpty(queueADT q);
size_t queueSize(queueADT q);
size_t countOccurrences(queueADT q, void *elem);

#endif
