#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <drivers/videoDriver.h>
#include <interrupts/idtLoader.h>
#include <interrupts/kernel_syscalls.h>
#include <drivers/time.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

int main()
{	
	load_idt();

	((EntryPoint)sampleCodeModuleAddress)();

	return 0;
}
