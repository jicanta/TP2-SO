#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include "time.h"
#include <lib.h>

typedef struct {
    char * name;
    char * description;
    void (*function)();
} COMMAND;


void execute(char * name);

void help_function();

char * fillCommand(char * buffer, unsigned int len);

void zoomin_function();

void zoomout_function();

void clear_function();

void registers_function();

void time_function();

void memory_status_function();

void malloc_function();

// ASM

void dispatcherGetRegisters();

void dispatcherGetTime(time_struct * t);

void dispatcherIncScale();

void dispatcherDecScale();

void check_no_opcode();

void check_0_div();

void dispatcherMemStatus(MemoryStatus * status);

void dispatcherFree(void *memorySegment);

#endif