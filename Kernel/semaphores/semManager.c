#include "../include/semManager.h"
#include "../include/videoDriver.h"

sem_t sems[MAX_SEMS];

int initSemManager(){

    for (int i = 0; i < MAX_SEMS; i++) {
        sems[i].used = 0;
        sems[i].locked = 0;
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

int findSemByName(const char * name){
    for (int i = 0; i < MAX_SEMS; i++) {
        if (strcmp(sems[i].name, name) == 0) {
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

int semCreate(const char * name, int value){
    int nameLen = strlen(name);
    if (nameLen > SEM_MAX_NAME_LENGTH || findSemByName(name) != -1){
        return -1;
    }

    int semId;
    semId = findFreeSem();
    if (semId < 0) {
        return -1;
    }
    
    sems[semId].value = value;
    sems[semId].used = 1;
    memcpy(sems[semId].name, name, nameLen);

    for (int i = 0; i < MAX_PROCESSES; i++){
        sems[semId].openedBy[i] = 0;
    }
    sems[semId].openedBy[getpid()] = 1;

    return semId;
}

int semOpen(const char * name){
    
    int semId = findSemByName(name);

    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return -1;
    }

    sems[semId].openedBy[getpid()] = 1;
    
    return semId;
}

int semClose(int semId){

    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0 || sems[semId].openedBy[getpid()] == 0) {
        return -1;
    }

    sems[semId].openedBy[getpid()] = 0;

    return 0;
}

int semWait(int semId){
    
    spinlockAcquire(&sems[semId].locked);
    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        spinlockRelease(&sems[semId].locked);
        return -1;
    }

    while (sems[semId].value == 0) {
        PID currentProcess = getpid();
        queue(sems[semId].waiting, currentProcess);
        
        spinlockRelease(&sems[semId].locked);

        blockProcess(currentProcess);
    
        spinlockAcquire(&sems[semId].locked);
    }

    sems[semId].value--;
    
    spinlockRelease(&sems[semId].locked);

    return 0;
}

int semPost(int semId){

    spinlockAcquire(&sems[semId].locked);

    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        spinlockRelease(&sems[semId].locked);
        return -1;
    }

    sems[semId].value++;
    
    while (!isEmpty(sems[semId].waiting)) {
        PID pid = dequeue(sems[semId].waiting);
        unblockProcess(pid);
    }

    spinlockRelease(&sems[semId].locked);

    return 0;
}

int semValue(int semId){
    if (!semExists(semId)){
        return -1;
    }
    return sems[semId].value;
}

int semDestroy(int semId){

    if (semId < 0) {
        spinlockRelease(&sems[semId].locked);
        return -1;
    }

    spinlockAcquire(&sems[semId].locked);

    sems[semId].used = 0;

    while (!isEmpty(sems[semId].waiting)) {
        PID pid = dequeue(sems[semId].waiting);
        unblockProcess(pid);
    }

    freeQueue(sems[semId].waiting);

    spinlockRelease(&sems[semId].locked);

    return 0;
}

