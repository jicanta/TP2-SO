#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoDriver.h>
#include <fonts.h>
#include "include/time.h"
#include <defs.h>
#include "interrupts.h"
#include "time.h"

#include "memoryManager.h"
#include "process.h"
#include "scheduler.h"
#include "semManager.h"
#include "fileDescriptors.h"


void load_idt(void);

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax);


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;
static void *const memoryStart = (void *)0xF00000;
const int memorySize = (1 << 20); // 1GB

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}

void idle()
{
	while (1)
	{
		_hlt();
		//vdPrint("Idle process running...\n", 0xFFFFFF);
	}
}


int main() {

	load_idt();

	createMemoryManager(memoryStart, memorySize);

	initializeVideoDriver();
	initFontManager();
	vdShowBootScreen();
	initializeTimer();
	initSemManager();
	
	vdClearScreen();
	//initializeSems();
	initProcesses();
	initFileDescriptors();

	creationParameters params;
	params.name = "init";
	params.argc = 0;
	params.argv = NULL;
	params.priority = DEFAULT_PRIORITY;
	params.entryPoint = (entryPoint)&idle;
	params.foreground = 0;
	params.fds[0] = STDIN;
	params.fds[1] = STDOUT;
	createProcess(&params);
	initScheduler();
	params.name = "shell";
	params.entryPoint = (entryPoint)sampleCodeModuleAddress;
	params.foreground = 1;
	params.argc = 0;
	params.argv = NULL;
	params.priority = DEFAULT_PRIORITY;
	params.fds[0] = STDIN;
	params.fds[1] = STDOUT;
	createProcess(&params);
	forceSwitchContent();
	
}