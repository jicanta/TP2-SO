/* sampleCodeModule.c */
#include <stdint.h>
#include "include/syscalls.h"
#include "include/stdio.h"
#include "include/shell.h"

int main() {
	init();
	return 0xDEADBEEF;
}