#include "../include/process.h"
#include "../include/memoryManager.h"
#include <stddef.h>

#define STACK_SIZE 4096 // Tamaño del stack para cada proceso

static uint64_t nextPid = 0;

PCB processTable[2]; // Definición real, solo acá

//ESTA FUNCION ANDA BIEN

uint64_t createProcess(entryPoint entry) {

     if (entry == NULL || nextPid >= 2)
        return -1;

    PCB *process = &processTable[nextPid];
    process->pid = nextPid;
    process->stackPointer = (uint64_t *)allocMemory(STACK_SIZE);
    if (process->stackPointer == NULL)
        return -1;

    process->stackPointer = (uint64_t *)((uint8_t *)process->stackPointer + STACK_SIZE);
    *(--process->stackPointer) = (uint64_t)entry;

    if(nextPid == 1){
         for (int i = 0; i < 6; i++){
        *(--process->stackPointer) = 0;
    }
    }
        
    

   
    process->state = 0; // READY

    vdPrint("Creando proceso con PID: ", 0xFFFFFF);
    vdPrintInt(process->pid, 0xFFFFFF);
    vdPrint("\n", 0xFFFFFF);

    //simula un sleep con un for
    
    for (volatile int i = 0; i < 400000000; i++); // Simular delay

    nextPid++;

    return process->pid;
}