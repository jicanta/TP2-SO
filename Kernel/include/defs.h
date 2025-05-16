/***************************************************
  Defs.h
****************************************************/

#ifndef _defs_
#define _defs_

#include <stdint.h>


/* Flags for segment access rights */
#define ACS_PRESENT     0x80            /* segment present in memory */
#define ACS_CSEG        0x18            /* code segment */
#define ACS_DSEG        0x10            /* data segment */
#define ACS_READ        0x02            /* read segment */
#define ACS_WRITE       0x02            /* write segment */
#define ACS_IDT         ACS_DSEG
#define ACS_INT_386 	0x0E		/* Interrupt GATE 32 bits */
#define ACS_INT         ( ACS_PRESENT | ACS_INT_386 )


#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define BYTE_LENGHT      8
#define TWO_BYTE_LENGHT  16

#define NULL 0

#define STACK_SIZE 4096

typedef int64_t pid_t;

typedef struct {
    uint32_t total;  
    uint32_t used;   
    uint32_t free;    
    void    *base;    
    void    *end;     
} MemoryStatus;

// TODO: Crear PCB.h

#define MAX_NAME_LENGTH 24
typedef int (*entryPoint)(int argc, char *argv[]);

typedef enum {
    READY = 0,
    RUNNING,
    BLOCKED,
    DEAD,
} processState;

typedef struct {
    char name[MAX_NAME_LENGTH];
    pid_t pid, parentPid, waitingPid;
    uint32_t argc;
    char **argv;
    uint32_t priority;
    entryPoint entryPoint;
    int foreground;
    processState state;
    uint64_t *stackBase, *stackEnd;
    int childReturnValue;
    int fds[2];
} PCB;

#endif