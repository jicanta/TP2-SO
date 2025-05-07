#include <stdint.h>
#include <stdlib.h>
#include "CuTest.h"
#include "DummyTest.h"

void testSimpleAddition(CuTest *const cuTest);

static const size_t TestQuantity = 1;
static const Test ArithmeticTests[] = {testSimpleAddition};

static inline void givenTwoNumbers();
static inline void whenNumbersAreAdded();
static inline void thenResultIsCorrect(CuTest *const cuTest);

static int firstNumber;
static int secondNumber;
static int result;

CuSuite *getDummyTestSuite(void) {
    CuSuite *const suite = CuSuiteNew();

    for (size_t i = 0; i < TestQuantity; i++)
        SUITE_ADD_TEST(suite, ArithmeticTests[i]);

    return suite;
}

void testSimpleAddition(CuTest *const cuTest) {
    givenTwoNumbers();
    whenNumbersAreAdded();
    thenResultIsCorrect(cuTest);
}

inline void givenTwoNumbers() {
    firstNumber = 5;
    secondNumber = 3;
}

inline void whenNumbersAreAdded() {
    result = firstNumber + secondNumber;
}

inline void thenResultIsCorrect(CuTest *const cuTest) {
    CuAssertIntEquals(cuTest, 8, result);
}
