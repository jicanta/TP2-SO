<<<<<<< HEAD
=======
#include "include/kernelManager.h"
#include "include/process.h"
#include "include/scheduler.h"

void initProcess(){
	while(1){
		_hlt();
    }
    return;
}

void initManagement(void){
    schedulerADT scheduler = initScheduler();
    processManagerADT processManager = initProcessManager(scheduler);

    char* argv[] = {"init", NULL};
    pid_t initPid = createProcess(processManager, 0, (uint64_t (*)(char **, int))initProcess, argv);

    if (initPid > 0) {
        processControlBlockADT initPcb = getProcess(processManager, initPid);
        if (initPcb) {
            initPcb->state = STATE_READY;
            scheduleProcess(scheduler, initPcb);
        }
    }
}
>>>>>>> parent of 592ca2b (Agrego test de la catedra)
