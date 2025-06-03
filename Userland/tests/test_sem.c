#include "test.h"
#include "test_util.h"
#include "../SampleCodeModule/include/lib.h"
#include "../SampleCodeModule/include/stdio.h"
#include "../SampleCodeModule/include/syscalls.h"

void sync_test(uint64_t argc, char *argv[]){
    int sem1;
    
    PID pid = getPid();

    printf("Process ID: %d\n", pid);

    sem1 = semOpen("/sem1", 0);
    //semOpen(&sem2, 1);
    
    printf("SEM1: ID: %d, VALUE: %d\n", sem1, semValue(sem1));
    semPost(sem1);
    printf("SEM1: ID: %d, VALUE: %d\n", sem1, semValue(sem1));
    semWait(sem1);
    printf("SEM1: ID: %d, VALUE: %d\n", sem1, semValue(sem1));

    printf("\n");

    //printf("SEM2: ID: %d, VALUE: %d\n", sem2, semValue(sem2));

    return;
}

uint64_t test_sem(uint64_t argc, char *argv[]){
    
    PID pid1;
    creationParameters params1;
    params1.name = "sync1";
    params1.argc = 0;
    params1.argv = NULL;
    params1.priority = 1;
    params1.entryPoint = (entryPoint)sync_test;
    params1.foreground = 1;
    pid1 = createProcess(&params1);
    printf("PID: %d\n", pid1);
    //sysWait(pid1, NULL);    // TODO: Chequear

    PID pid2;
    creationParameters params2;
    params2.name = "sync2";
    params2.argc = 0;
    params2.argv = NULL;
    params2.priority = 1;
    params2.entryPoint = (entryPoint)sync_test;
    params2.foreground = 1;
    pid2 = createProcess(&params2);
    printf("PID: %d\n", pid2);
    //sysWait(pid2, NULL);
    
    return 0;
}