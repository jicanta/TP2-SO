#include "test.h"
#include "test_util.h"
#include "../SampleCodeModule/include/lib.h"
#include "../SampleCodeModule/include/stdio.h"
#include "../SampleCodeModule/include/syscalls.h"

void sync_test(uint64_t argc, char *argv[]){

    for (int i = 0; i < 100; i++){
        printf("%d\n", i);
    }

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
    sysWait(pid1, NULL);    // TODO: Chequear

    PID pid2;
    creationParameters params2;
    params2.name = "sync2";
    params2.argc = 0;
    params2.argv = NULL;
    params2.priority = 1;
    params2.entryPoint = (entryPoint)sync_test;
    params2.foreground = 1;
    pid2 = createProcess(&params2);
    sysWait(pid2, NULL);

    return 0;
}