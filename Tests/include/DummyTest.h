#ifndef DUMMY_TEST
#define DUMMY_TEST

#include "CuTest.h"

typedef void (*Test)(CuTest *const cuTest);

CuSuite *getDummyTestSuite(void);

#endif