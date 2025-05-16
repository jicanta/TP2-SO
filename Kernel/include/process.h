#ifndef __PROCESS_H
#define __PROCESS_H

#define MAX_PROCESSES 10
#define MAX_PRIORITY 5
#define MIN_PRIORITY 1
#define DEFAULT_PRIORITY MIN_PRIORITY
#define MAX_PID MAX_PROCESSES

#include <defs.h>
#include <stdint.h>

pid_t initProcesses(void);
pid_t createProcess(const char *name, uint32_t argc, char *argv[], uint32_t priority, entryPoint entryPoint, int foreground);

#endif