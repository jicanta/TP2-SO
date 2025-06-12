// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>

#include "CuTest.h"
#include "DummyTest.h"
#include "MemoryManagerTest.h"
#include "QueueTest.h"

void RunAllTests(void) {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, getDummyTestSuite());
	CuSuiteAddSuite(suite, getNaiveMemoryManagerTestSuite());
	CuSuiteAddSuite(suite, getBuddyMemoryManagerTestSuite());
	CuSuiteAddSuite(suite, getQueueTestSuite());

	CuSuiteRun(suite);

	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);

	printf("%s\n", output->buffer);
}

int main(void) {
	RunAllTests();
	return 0;
}