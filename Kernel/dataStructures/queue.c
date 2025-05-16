#include "queueADT.h"
#include "memoryManager.h"

static queueNode *newNode(void *data) {
    queueNode *n = allocMemory(sizeof(queueNode));
    if (!n) {
        return NULL;
    }
    n->data = data;
    n->next = NULL;
    return n;
}

queueADT createQueue(void) {
    queueADT q = allocMemory(sizeof(struct queueCDT));
    if (!q) {
        return NULL;
    }
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}

void destroyQueue(queueADT q) {
    if (!q) {
        return;
    }
    queueNode *it = q->head;
    while (it) {
        queueNode *next = it->next;
        freeMemory(it);
        it = next;
    }
    freeMemory(q);
}

void enqueue(queueADT q, void *elem) {
    if (!q) {
        return;
    }
    queueNode *n = newNode(elem);
    if (!n) {
        return;
    }

    if (q->tail) {
        q->tail->next = n;
    } else {
        q->head = n;
    }
    q->tail = n;
    q->size++;
}

void *dequeue(queueADT q) {
    if (!q || !q->head) {
        return NULL;
    }
    queueNode *n = q->head;
    void *data = n->data;

    q->head = n->next;
    if (!q->head) {
        q->tail = NULL;
    }
    freeMemory(n);
    q->size--;
    return data;
}

void *dequeueValue(queueADT q, void *elem) {
    if (!q) {
        return NULL;
    }
    queueNode *prev = NULL;
    queueNode *cur = q->head;

    while (cur && cur->data != elem) {
        prev = cur;
        cur  = cur->next;
    }
    if (!cur) {
        return NULL;
    }

    if (prev) {
        prev->next = cur->next;
    } else {
        q->head = cur->next;
    }
    if (cur == q->tail) {
        q->tail = prev;
    }

    void *data = cur->data;
    freeMemory(cur);
    q->size--;
    return data;
}

int queueIsEmpty(queueADT q) {
    return !q || q->size == 0;
}

uint64_t queueSize(queueADT q) {
    return q ? q->size : 0;
}

uint64_t countOccurrences(queueADT q, void *elem) {
    uint64_t count = 0;
    for (queueNode *n = q ? q->head : NULL; n; n = n->next) {
        if (n->data == elem) {
            count++;
        }
    }
    return count;
}
