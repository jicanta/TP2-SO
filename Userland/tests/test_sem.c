#include "test.h"
#include "test_util.h"
#include "SampleCodeModule/include/lib.h"
#include "SampleCodeModule/include/stdio.h"

void sync_test(uint64_t argc, char *argv[]){

    for (int i = 0; i < 100; i++){
        printf("%d\n", i);
    }

    return;
}

uint64_t test_sem(uint64_t argc, char *argv[]){
    
    PID pid1;
    creationParameters params;
    params.name = "sync1";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)sync_test;
    params.foreground = 1;
    pid1 = createProcess(&params);
    sysWait(pid1, NULL);

    PID pid1;
    creationParameters params;
    params.name = "sync2";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)sync_test;
    params.foreground = 1;
    pid1 = createProcess(&params);
    sysWait(pid1, NULL);

    return 0;
}