#include "include/semManager.h"


sem_t sems[MAX_SEMS];

uint32_t initSemMaager(){

    for (int i = 0; i < MAX_SEMS; i++) {
        sems[i].used = 0;
        sems[i].waiting = newQueue();
        if (sems[i].waiting == NULL) {
            return -1;
        }
    }

    return 0;
}

static uint32_t findFreeSem() {
    for (uint32_t i = 0; i < MAX_SEMS; i++) {
        if (sems[i].used == 0) {
            return i;
        }
    }
    return -1;
}


uint32_t semInit(uint32_t value){
    uint32_t semId;

    semId = findFreeSem();

    sems[semId].value = value;
    sems[semId].used = 1;

    return semId;
}

uint32_t semWait(uint32_t semId){
    
    // TODO: TOMAR SEMAFORO! (acquire)
    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return -1; // Invalid semaphore ID
    }

    sem_t sem = sems[semId];

    while(sem.value == 0) {
        PID currentProcess = getpid();
        queue(sems[semId].waiting, currentProcess);
        
        // TODO: SOLTAR SEMAFORO! (release)
        blockProcess(currentProcess);
    
        // TODO: TOMAR SEMAFORO! (acquire)
    }

    sem.value--;
    
    // TODO: SOLTAR SEMAFORO! (release)

    return 0;
}

uint32_t semPost(uint32_t semId){
    
    // TODO: TOMAR SEMAFORO! (acquire)

    if (semId < 0 || semId >= MAX_SEMS || sems[semId].used == 0) {
        return -1; // Invalid semaphore ID
    }

    sems[semId].value++;
    while (!isEmpty(sems[semId].waiting)) {
        PID pid = dequeue(sems[semId].waiting);
        unblockProcess(pid);
    }

    // TODO: SOLTAR SEMAFORO! (release)

    return 0;
}

uint32_t semDestroy(uint32_t semId){
    sems[semId].used = 0;

    while (!isEmpty(sems[semId].waiting)) {
        PID pid = dequeue(sems[semId].waiting);
        unblockProcess(pid);
    }

    freeQueue(sems[semId].waiting);

    return 0;
}
