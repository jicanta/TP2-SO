#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <drivers/videoDriver.h>
#include <interrupts/idtLoader.h>
#include <interrupts/kernel_syscalls.h>
#include <drivers/time.h>
#include <memoryManager.h>
#include "../include/memory/memoryManager.h"

// Function prototype for test_mm
uint64_t test_mm();

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

static void *const memoryStartAddress = (void *)0xF00000;
const int memorySize = (1 << 20);

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
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
	char buffer[10];

	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int main()
{
	createMemoryManager(memoryStartAddress, memorySize);	
	load_idt();
	if(test_mm() < 0) {
    while(1);
  }


	((EntryPoint)sampleCodeModuleAddress)();

	return 0;
}

// NOTA: Testeo de memory manager desde kernel.c.
// No va aca, es para testeo nomas

static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint() {
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max) {
  uint32_t u = GetUint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t memcheck(void *start, uint8_t value, uint32_t size) {
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
      return 0;

  return 1;
}

void *my_memset(void *destination, int32_t c, uint64_t length) {
  uint8_t chr = (uint8_t)c;
  char *dst = (char *)destination;
  while (length--) {
    dst[length] = chr;
  }
  return destination;
}

// Parameters
int64_t satoi(char *str) {
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str) {
    return 0;
  }
  if (str[i] == '-') {
    i++;
    sign = -1;
  }
  for (; str[i] != '\0'; ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}

#define MAX_BLOCKS 10

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm()
{
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

 
  if ((max_memory = satoi("25000")) <= 0)
    return -1;

  int iter = 0;
  while (iter++ < 10000)
  {

    rq = 0;
    total = 0;
    while (rq < MAX_BLOCKS && total < max_memory)
    {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = allocMemory(mm_rqs[rq].size);
      if (mm_rqs[rq].address)
      {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    uint32_t i;
    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
    }

    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
        {
          return -1;
        }
    }

    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
        freeMemory(mm_rqs[i].address);
    }
  }
  return 1;
}