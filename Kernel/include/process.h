#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>


typedef struct ProcessControlBlock {
    uint64_t pid;               // ID del proceso
    uint64_t *stackPointer;     // Puntero al stack del proceso
    uint64_t state;             // Estado del proceso (READY, RUNNING, etc.)
} PCB;

extern PCB processTable[2];


typedef uint64_t (*entryPoint)(void); // Tipo de función para el proceso

// Función para crear un proceso
uint64_t createProcess(entryPoint entry);

#endif