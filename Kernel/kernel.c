#include <stdint.h>
#include <string.h>
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


void load_idt(void);

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax);


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

uint64_t testProcess(void) {
    while (1) {
        vdPrint("Proceso de prueba ejecutándose...\n", 0xFFFFFF);
        for (volatile int i = 0; i < 1000000; i++); // Simular delay
    }
    return 0;
}

uint64_t shellProcess(void) {
    while (1) {
        vdPrint("Shell ejecutándose...\n", 0x00FF00);
        //for (volatile int i = 0; i < 1000000; i++);
        schedule(); // Cede el control al otro proceso
    }
    return 0;
}

uint64_t idleProcess(void) {
    while (1) {
        vdPrint("idle ejecutándose...\n", 0xFF0000);
        //for (volatile int i = 0; i < 1000000; i++);
        schedule(); // Cede el control al otro proceso
    }
    return 0;
}

extern void startFirstProcess(uint64_t *stackPointer);

int main() {
	load_idt();
	//initializeTimer();	
	initializeVideoDriver();
	initFontManager();

	createMemoryManager(&endOfKernel, 0x100000); // Inicializar el gestor de memoria

    createProcess(idleProcess);
	    
    createProcess(shellProcess);

	//schedule(); // Iniciar el primer proceso

    //startFirstProcess(processTable[0].stackPointer);

	idleProcess();

	//((EntryPoint)*(process->stackPointer))();

	//vdPrintInt((uint64_t)process->stackPointer, 0xFF0000);

	//vdPrint("Fin de la ejecución del kernel\n", 0xFFFFFF);
    //while (1) _hlt();
    return 0;
	
}