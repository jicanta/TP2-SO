#include "test.h"
#include "test_util.h"
#include "../SampleCodeModule/include/lib.h"
#include "../SampleCodeModule/include/stdio.h"
#include "../SampleCodeModule/include/syscalls.h"

void sync_test1(uint64_t argc, char *argv[]){
    
    PID pid = getPid();

    int sem0 = semOpen("/sem0", 1);
    int sem1 = semOpen("/sem1", 0);
    
    for (int i = 0; i < 6; i++){
        semWait(sem0);
        printf("PID: %d\n", pid);
        semPost(sem1);
    } 

    
    return;
}

void sync_test2(uint64_t argc, char *argv[]){
    
    PID pid = getPid();

    int sem0 = semOpen("/sem0", 1);
    int sem1 = semOpen("/sem1", 0);
    
    for (int i = 0; i < 6; i++){
        semWait(sem1);
        printf("PID: %d\n", pid);
        semPost(sem0);
    }
    



    return;
}

uint64_t test_sem(uint64_t argc, char *argv[]){
    /*
    PID pid1;
    creationParameters params1;
    params1.name = "sync1";
    params1.argc = 0;
    params1.argv = NULL;
    params1.priority = 1;
    params1.entryPoint = (entryPoint)sync_test1;
    params1.foreground = 1;
     params1.fds[0] = STDIN; // Lectura
    params1.fds[1] = STDOUT; // Escritura
    pid1 = createProcess(&params1);
    
    PID pid2;
    creationParameters params2;
    params2.name = "sync2";
    params2.argc = 0;
    params2.argv = NULL;
    params2.priority = 1;
    params2.entryPoint = (entryPoint)sync_test2;
    params2.foreground = 1;
     params2.fds[0] = STDIN; // Lectura
    params2.fds[1] = STDOUT; // Escritura
    pid2 = createProcess(&params2);
    
    sysWait(pid1, NULL);
    sysWait(pid2, NULL);
    
    return 0;*/
}