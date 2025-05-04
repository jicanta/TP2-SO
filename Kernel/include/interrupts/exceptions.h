#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <stdint.h>
#include <lib.h>

void exceptionDispatcher(uint64_t exception);

static void zero_division();

#define REGS_LEN 18

#endif