// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "CuTest.h"

typedef long type;

typedef struct node {
    type data;
    struct node *tail;
} node;

typedef struct queueCDT {
    node *first;
    node *last;
} queueCDT, *queueADT;

static queueADT newQueue(void);
static void queue(queueADT q, type element);
static type dequeue(queueADT q);
static uint32_t isEmpty(queueADT q);
static void freeQueue(queueADT q);

static queueADT newQueue(void) {
    queueADT q = (queueADT)malloc(sizeof(queueCDT));
    if (q == NULL) return NULL;
    q->first = q->last = NULL;
    return q;
}

static void queue(queueADT q, type element) {
    node *n = (node *)malloc(sizeof(node));
    if (n == NULL) return;
    n->data = element;
    n->tail = NULL;

    if (q->first == NULL) {
        q->first = q->last = n;
    } else {
        q->last->tail = n;
        q->last = n;
    }
}

static type dequeue(queueADT q) {
    if (q->first == NULL) return -1;

    node *tmp = q->first;
    type val = tmp->data;
    q->first = tmp->tail;
    if (q->first == NULL) q->last = NULL;
    free(tmp);
    return val;
}

static uint32_t isEmpty(queueADT q) {
    return (q->first == NULL);
}

static void freeQueue(queueADT q) {
    if (q == NULL) return;
    while (!isEmpty(q)) (void)dequeue(q);
    free(q);
}

typedef void (*Test)(CuTest *const tc);

static void testNewQueueIsEmpty(CuTest *const tc);
static void testEnqueueMakesNotEmpty(CuTest *const tc);
static void testFIFOOrder(CuTest *const tc);
static void testDequeueEmptySentinel(CuTest *const tc);
static void testEnqueueMany(CuTest *const tc);
static void testInterleavedOps(CuTest *const tc);
static void testFreeQueueAfterPartialUse(CuTest *const tc);
static void testStress1024(CuTest *const tc);
static void testDoubleFreeIdempotent(CuTest *const tc);

static const Test QueueTests[] = {
    testNewQueueIsEmpty,
    testEnqueueMakesNotEmpty,
    testFIFOOrder,
    testDequeueEmptySentinel,
    testEnqueueMany,
    testInterleavedOps,
    testFreeQueueAfterPartialUse,
    testStress1024,
    testDoubleFreeIdempotent
};

CuSuite *getQueueTestSuite(void) {
    CuSuite *const suite = CuSuiteNew();
    for (size_t i = 0; i < sizeof QueueTests / sizeof QueueTests[0]; ++i)
        SUITE_ADD_TEST(suite, QueueTests[i]);
    return suite;
}

static queueADT q;
static uint32_t flag;
static int32_t value;
static int32_t buf[1024];

static inline void givenAFreshQueue(void) { q = newQueue(); }
static inline void whenWeAskIfEmpty(void) { flag = isEmpty(q); }
static inline void thenQueueIsEmpty(CuTest *tc) { CuAssertTrue(tc, flag); freeQueue(q); }

static void testNewQueueIsEmpty(CuTest *const tc) {
    givenAFreshQueue(); whenWeAskIfEmpty(); thenQueueIsEmpty(tc);
}

static inline void givenAnEmptyQueue(void) { q = newQueue(); }
static inline void whenWeEnqueueOneElement(void) { queue(q, 99); flag = isEmpty(q); }
static inline void thenQueueIsNotEmpty(CuTest *tc) { CuAssertTrue(tc, !flag); freeQueue(q); }

static void testEnqueueMakesNotEmpty(CuTest *const tc) {
    givenAnEmptyQueue(); whenWeEnqueueOneElement(); thenQueueIsNotEmpty(tc);
}

static int32_t out[3];

static inline void givenThreeElements(void) {
    q = newQueue();
    queue(q, 1); queue(q, 2); queue(q, 3);
}

static inline void whenWeDequeueThreeTimes(void) {
    out[0] = dequeue(q); out[1] = dequeue(q); out[2] = dequeue(q);
}

static inline void thenFIFO(CuTest *tc) {
    CuAssertIntEquals(tc, 1, out[0]);
    CuAssertIntEquals(tc, 2, out[1]);
    CuAssertIntEquals(tc, 3, out[2]);
    CuAssertTrue(tc, isEmpty(q));
    freeQueue(q);
}

static void testFIFOOrder(CuTest *const tc) {
    givenThreeElements(); whenWeDequeueThreeTimes(); thenFIFO(tc);
}

static inline void givenEmptyAgain(void) { q = newQueue(); }
static inline void whenWeDequeueEmpty(void) { value = dequeue(q); }
static inline void thenMinusOne(CuTest *tc) {
    CuAssertIntEquals(tc, -1, value);
    CuAssertTrue(tc, isEmpty(q));
    freeQueue(q);
}

static void testDequeueEmptySentinel(CuTest *const tc) {
    givenEmptyAgain(); whenWeDequeueEmpty(); thenMinusOne(tc);
}

static inline void givenHundred(void) {
    q = newQueue();
    for (int i = 0; i < 100; ++i) queue(q, i);
}

static inline void whenWeDrainHundred(void) {
    for (int i = 0; i < 100; ++i) buf[i] = dequeue(q);
}

static inline void thenHundredFIFO(CuTest *tc) {
    for (int i = 0; i < 100; ++i)
        CuAssertIntEquals(tc, i, buf[i]);
    CuAssertTrue(tc, isEmpty(q));
    freeQueue(q);
}

static void testEnqueueMany(CuTest *const tc) {
    givenHundred(); whenWeDrainHundred(); thenHundredFIFO(tc);
}

static inline void givenInterleaved(void) {
    q = newQueue();
    queue(q, 1); queue(q, 2);
    buf[0] = dequeue(q);
    queue(q, 3); queue(q, 4);
    buf[1] = dequeue(q);
    buf[2] = dequeue(q);
    buf[3] = dequeue(q);
}

static inline void whenInterleavedDone(void) {}

static inline void thenInterleavedSequence(CuTest *tc) {
    CuAssertIntEquals(tc, 1, buf[0]);
    CuAssertIntEquals(tc, 2, buf[1]);
    CuAssertIntEquals(tc, 3, buf[2]);
    CuAssertIntEquals(tc, 4, buf[3]);
    CuAssertTrue(tc, isEmpty(q));
    freeQueue(q);
}

static void testInterleavedOps(CuTest *const tc) {
    givenInterleaved(); whenInterleavedDone(); thenInterleavedSequence(tc);
}

static size_t beforeFP, afterFP;

static inline size_t heapMarker(void) {
    void *p = malloc(1);
    size_t m = (size_t)p;
    free(p);
    return m;
}

static inline void givenHalfConsumed(void) {
    beforeFP = heapMarker();
    q = newQueue();
    for (int i = 0; i < 20; ++i) queue(q, i);
    for (int i = 0; i < 10; ++i) (void)dequeue(q);
}

static inline void whenWeFreeAll(void) {
    freeQueue(q);
    afterFP = heapMarker();
}

static inline void thenHeapSame(CuTest *tc) {
    size_t diff = (beforeFP > afterFP) ? beforeFP - afterFP : afterFP - beforeFP;
    CuAssertTrue(tc, diff < 1024u);
}

static void testFreeQueueAfterPartialUse(CuTest *const tc) {
    givenHalfConsumed(); whenWeFreeAll(); thenHeapSame(tc);
}

static inline void given1024(void) {
    q = newQueue();
    for (int i = 0; i < 1024; ++i) queue(q, i);
}

static inline void whenDrain1024(void) {
    for (int i = 0; i < 1024; ++i) buf[i] = dequeue(q);
}

static inline void thenAll1024(CuTest *tc) {
    for (int i = 0; i < 1024; ++i)
        CuAssertIntEquals(tc, i, buf[i]);
    CuAssertTrue(tc, isEmpty(q));
    freeQueue(q);
}

static void testStress1024(CuTest *const tc) {
    given1024(); whenDrain1024(); thenAll1024(tc);
}

static inline void givenFreedOnce(void) {
    q = newQueue();
    for (int i = 0; i < 5; ++i) queue(q, i);
    freeQueue(q);
    q = NULL;
}

static inline void whenWeFreeAgain(void) { freeQueue(q); }
static inline void thenNoCrash(CuTest *tc) { CuAssertTrue(tc, 1); }

static void testDoubleFreeIdempotent(CuTest *const tc) {
    givenFreedOnce(); whenWeFreeAgain(); thenNoCrash(tc);
}