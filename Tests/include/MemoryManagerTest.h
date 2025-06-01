#ifndef MEMORY_MANAGER_TEST_H
#define MEMORY_MANAGER_TEST_H

#include "CuTest.h"
#include "memoryManager.h"

CuSuite *getNaiveMemoryManagerTestSuite(void);
CuSuite *getBuddyMemoryManagerTestSuite(void);

#endif // MEMORY_MANAGER_TEST_H