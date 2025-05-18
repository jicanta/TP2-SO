// scheduler.c
#include "../include/process.h"
#include "videoDriver.h"
#include "scheduler.h"

static int currentProcess = 0;

extern void contextSwitch(uint64_t **old_rsp, uint64_t *new_rsp);

void schedule() {
    vdPrint("Current process antes del cambio: ", 0xFFFFFF);
    vdPrintInt(currentProcess, 0xFFFFFF);
    vdPrint("\n", 0xFFFFFF);
    
    // Guarda el proceso actual
    int oldProcess = currentProcess;
    
    // Calcula y actualiza al próximo proceso
    currentProcess = 1 - currentProcess;
    
    
    // Opcional: delay más corto para debugging
    for (volatile int i = 0; i < 100000000; i++);

    vdPrint("Old stack: ", 0xFFFFFF);
vdPrintInt((uint64_t)processTable[oldProcess].stackPointer, 0xFFFFFF);
vdPrint(", New stack: ", 0xFFFFFF);
vdPrintInt((uint64_t)processTable[currentProcess].stackPointer, 0xFFFFFF);
vdPrint("\n", 0xFFFFFF);
    
    // Cambio de contexto del viejo al nuevo
    contextSwitch(&processTable[oldProcess].stackPointer, processTable[currentProcess].stackPointer);
}