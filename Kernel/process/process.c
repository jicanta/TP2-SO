#include "../include/process.h"
#include "../include/memoryManager.h"
#include <stddef.h>

#define STACK_SIZE 4096 // Tamaño del stack para cada proceso

static uint64_t nextPid = 0;

PCB processTable[1]; // Definición real, solo acá

//ESTA FUNCION ANDA BIEN

uint64_t createProcess(entryPoint entry) {

    if (entry == NULL) {
        return -1; // Error: entrada inválida
    }

    //vdPrintInt(nextPid, 0xFFFFFF); // Imprimir PID para depuración

    PCB *process = &processTable[0]; // Usamos un solo proceso para pruebas
    process->pid = nextPid++;
    process->stackPointer = (uint64_t *)allocMemory(STACK_SIZE); // Asignar stack
    if (process->stackPointer == NULL) {
        return -1; // Error: no hay memoria para el stack
    }

    // Inicializar el stack del proceso
    process->stackPointer = (uint64_t *)((uint8_t *)process->stackPointer + STACK_SIZE); // Apuntar al final del stack
        //vdPrintInt((uint64_t)process->stackPointer, 0xFF0000); // Imprimir dirección del stack para depuración

    *(--process->stackPointer) = (uint64_t)entry; // Dirección de retorno (entryPoint)
    process->state = 1; // READY

    vdPrintInt(*(process->stackPointer), 0xFF0000); // Imprimir dirección del stack para depuración

    return process->pid;
}