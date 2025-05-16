#ifndef __PROCESS_H
#define __PROCESS_H

#define MAX_PROCESSES 10
#define MAX_PRIORITY 5
#define MIN_PRIORITY 1
#define DEFAULT_PRIORITY MIN_PRIORITY
#define MAX_PID MAX_PROCESSES

#include <defs.h>
#include <stdint.h>

#define INIT_PID 1

void initProcessManager(void);

pid_t createProcess(const char *name, uint32_t argc, char *argv[], uint32_t priority, entryPoint entryPoint, int foreground);
pid_t createInit(void);
uint8_t processExists(pid_t pid);

uint32_t blockProcess(pid_t pid);
uint32_t unblockProcess(pid_t pid);
uint32_t killProcess(pid_t pid);

#endif