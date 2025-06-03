#include "../include/semManager.h"
#include "../include/videoDriver.h"

sem_t sems[MAX_SEMS];

int initSemManager(){

    for (int i = 0; i < MAX_SEMS; i++) {
        sems[i].used = 0;
        sems[i].waiting = newQueue();
        if (sems[i].waiting == NULL) {
            return -1;
        }
    }

    return 0;
}

static int findFreeSem() {
    for (int i = 0; i < MAX_SEMS; i++) {
        if (sems[i].used == 0) {
            return i;
        }
    }
    return -1;
}

int semExists(int semId) {
    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return 0;
    }
    return 1;
}

int semCreate(int value){
    int semId;

    semId = findFreeSem();
    if (semId < 0) {
        return -1;
    }
    
    sems[semId].value = value;
    sems[semId].used = 1;

    for (int i = 0; i < MAX_PROCESSES; i++){
        sems[semId].openedBy[i] = 0;
    }
    sems[semId].openedBy[getpid()] = 1;

    return semId;
}

int semOpen(int semId){
    
    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return -1;
    }

    sems[semId].openedBy[getpid()] = 1;

    return 0;
}

int semClose(int semId){
    
    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0 || sems[semId].openedBy[getpid()] == 0) {
        return -1;
    }

    sems[semId].openedBy[getpid()] = 0;

    return 0;
}

int semWait(int semId){
    /*
    vdPrint("SemId: ", 0x00FFFFFF);
    vdPrintInt(semId, 0x00FFFFFF);
    vdPrint("\n", 0x00FFFFFF);
    */
    //spinlockAcquire(&sems[semId].used);
    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return -1;
    }

    sem_t sem = sems[semId];
    /*
    vdPrint("SemValue: ", 0x00FFFFFF);
    vdPrintInt(sem.value, 0x00FFFFFF);
    vdPrint("\n", 0x00FFFFFF);
    */
    while(sem.value == 0) {
        PID currentProcess = getpid();
        queue(sems[semId].waiting, currentProcess);
        
        //spinlockRelease(&sems[semId].used);

        blockProcess(currentProcess);
    
        //spinlockAcquire(&sems[semId].used);
    }

    sems[semId].value--;

    /*
    vdPrint("SemValue: ", 0x00FFFFFF);
    vdPrintInt(sem.value, 0x00FFFFFF);
    vdPrint("\n", 0x00FFFFFF);
    */
    
    //spinlockRelease(&sems[semId].used);

    return 0;
}

int semPost(int semId){
    
    //spinlockAcquire(&sems[semId].used);

    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return -1;
    }

    sems[semId].value++;
    vdPrint("   SemValue: ", 0x00FFFFFF);
    vdPrintInt(sems[semId].value, 0x00FFFFFF);
    vdPrint("\n", 0x00FFFFFF);
    while (!isEmpty(sems[semId].waiting)) {
        PID pid = dequeue(sems[semId].waiting);
        unblockProcess(pid);
    }

    //spinlockRelease(&sems[semId].used);

    return 0;
}

int semValue(int semId){
    if (!semExists){
        return -1;
    }
    return sems[semId].value;
}

int semDestroy(int semId){
    sems[semId].used = 0;

    while (!isEmpty(sems[semId].waiting)) {
        PID pid = dequeue(sems[semId].waiting);
        unblockProcess(pid);
    }

    freeQueue(sems[semId].waiting);

    return 0;
}

