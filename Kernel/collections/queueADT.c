#include "../include/queueADT.h"

typedef struct node {
    type data;
    struct node * tail;
} node;

typedef struct node * queueType;

typedef struct queueCDT {
    queueType first;
    queueType last;
} queueCDT;

queueADT newQueue() {
    queueADT q = (queueADT)allocMemory(sizeof(queueCDT));
    if (q == NULL) {
        return NULL; // Memory allocation failed
    }
    q->first = NULL;
    q->last = NULL;
    return q;
}

void queue(queueADT q, type element) {
    queueType newNode = (queueType)allocMemory(sizeof(node));
    newNode->data = element;
    newNode->tail = NULL;
    if (q->first == NULL) {
        q->first = newNode;
        q->last = newNode;
    } else {
        q->last->tail = newNode;
        q->last = newNode;
    }
}

type dequeue(queueADT q) {
    if (q->first == NULL) {
        return -1;
    }
    queueType temp = q->first;
    type data = temp->data;
    q->first = temp->tail;
    if (q->first == NULL) {
        q->last = NULL;
    }
    free(temp);
    return data;
}

uint32_t isEmpty(queueADT q) {
    return (q->first == NULL);
}

void freeQueue(queueADT q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
    free(q);
}