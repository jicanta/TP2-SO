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

// ASM

void _sys_get_registers();

void _sys_get_time(time_struct * t);

void _sys_inc_scale();

void _sys_dec_scale();

void check_no_opcode();

void check_0_div();

#endif