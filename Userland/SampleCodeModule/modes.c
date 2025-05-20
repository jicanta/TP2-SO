#include "include/modes.h"
#include "include/shell.h"
#include "include/stdio.h"
#include "include/string.h"
#include "include/syscalls.h"
#include "include/lib.h"
#include "include/exceptions.h"
#include "include/dateTime.h"
#include "include/colors.h"
#include "include/utils.h"
#include "include/stdio.h"
#include "include/memoryStructure.h"
#include "../tests/test.h"

char* dateTimeAux;
int zoomAux, regAux;

char *states[5] = {"Ready", "Running", "Blocked", "Dead", "Foreground"};

void help (void) {
    for(int i=0; strcasecmp(helpText[i], "end")!=0; i++){
        printColor(helpText[i], YELLOW);
    }
}



void clear (void) {
    sysClearScreen();
}

void time(void) {
    printTime();
}

void date(void) {
    printDate();
}

void zoomin() {
    zoomAux = incTextSize();
    if(zoomAux)
        print("Maximum size reached.\n");
}

void zoomout() {
    zoomAux = decTextSize();
    if(zoomAux)
        print("Minimum size reached.\n");
}

void divByZero() {
    divZero();
}

void invalidOp(){
    invalidOpcode();
}

void registers()  {
    regAux = sysPrintRegs();
    if(regAux){
        print("You need to save registers first by pressing ALT\n");
    }
}

void notFound(char* commandNotFound){
    print(commandNotFound); 
    print(": command not found.\n");
}

void playEasterEgg(){
    printColor("I'm", RED);
    printColor(" an", YELLOW);
    printColor(" eas", GREEN);
    printColor("ter ", BLUE);
    printColor("egg", WHITE);
    printColor("!\n", YELLOW);

    sysHideCursor();
    sysPrintCursor();

    sysBeepSound(220, DO);
	sysBeepSound(220, DO_SOST);
	sysBeepSound(220, SOL);
	sysBeepSound(220, MI);
	sysBeepSound(220, SOL);
	sysBeepSound(220, MI);
	
	sysBeepSound(220, DO_SOST);
	sysBeepSound(220, DO);
	sysBeepSound(220, SOL_SOST);
	sysBeepSound(220, FA);
	sysBeepSound(220, SOL_SOST);
	sysBeepSound(220, FA);

	sysBeepSound(220, DO);
	sysBeepSound(220, DO_SOST);
	sysBeepSound(220, SOL);
	sysBeepSound(220, MI);
	sysBeepSound(220, SOL);
	sysBeepSound(220, MI);

	sysBeepSound(110, FA);
	sysBeepSound(165, FA_SOST);
	sysBeepSound(110, SOL);
	sysBeepSound(165, SOL_SOST);
	sysBeepSound(110, LA);
	sysBeepSound(165, SI);
	sysBeepSound(110, DO_PRIMA);

    sysShowCursor();
    sysPrintCursor();
}

void ps(){
    Process *processes = sysGetPS();
     for (int i = 0; processes[i].pid != -1; i++)
    {
        printf("PID=%d | Name=",processes[i].pid);
        print(processes[i].name);
        printf(" | ParentPID=%d | Priority=%d | Foreground=%d | State=",
               processes[i].parentpid,
               processes[i].priority,
               processes[i].foreground);
        print(states[processes[i].state]);
        printf(" | StackBase=0x");
        print(hexToString((uint64_t)processes[i].stackBase));
        printf(" | StackEnd=0x");
        print(hexToString((uint64_t)processes[i].stackEnd));
        printf("\n");
    }
    sysFreePS(processes);
};

void printProcessesInformation()
{
    PID pid;
    creationParameters params;
    params.name = "ps";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)ps;
    params.foreground = 1;
    pid = createProcess(&params);
    sysWait(pid, NULL);
    return;
}

void printPidAndSayHi()
{
    

    while(1){
        printf("Hello! My PID is %d\n", sysGetPID());
        sysSleep(2, 0);
    }
}

// No se si esto esta bien, no entiendo si hay que crear el proceso 

void loop(){
    PID pid;
    creationParameters params;
    params.name = "loop";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)printPidAndSayHi;
    params.foreground = 1;
    pid = createProcess(&params);
    sysWait(pid, NULL);
    return;
}

//Recibe de parametro el pid del proceso a eliminar
//Elimina el proceso y libera la memoria

void kill(PID pid){
    if(pid == 1){
        printColor("You cannot kill the idle process.\n", RED);
        return;
    }
    
    else {

        //Este chequeo de pid == 2 no se si deberia estar en userland o en kernel
        //Lo dejo por ahora en userland

        if(pid == 2){
            printColor("Shell is going to be killed, reset QEMU to start again.\n", YELLOW);
        }
        int ret = sysKill(pid);
        if(ret == -1){
            printColor("Error eliminating process.\n", RED);
            return;
        }
        printColor("Process killed.\n", GREEN);
        return;
    }
}

void nice(PID pid, Priority newPriority){
    int ret = sysNice(pid,newPriority);

    if(ret == -1){
        printColor("Error changing priority\n",RED);
        return;
    }
    
    printColor("Priority changed\n",GREEN);
}

void block(PID pid){
    int ret = sysBlock(pid);

    if(ret == -1){
        printColor("Error blocking process\n",RED);
        return;
    }
    
    printColor("Process blocked\n",GREEN);

    
}

void printMemoryStatus(){
    MemoryStatus status; 
    sysGetMemStatus(&status);
    printf("Total : %d, usado : %d, libre : %d\n",status.total, status.used, status.free);
    printf("Direccion del inicio : %x, direccion del final : %x\n",status.base, status.end);
    
}

// TODO : Para testear (no estoy creando un proceso, hay que corregirlo)

void test(){
    //Crea el proceso
    PID pid;
    creationParameters params;
    params.name = "loop";
    params.argc = 1;
    char * argv[] = {"1000", NULL};
    params.argv = argv;
    params.priority = 1;
    params.entryPoint = (entryPoint)test_mm;
    params.foreground = 1;
    pid = createProcess(&params);
    sysWait(pid, NULL);
    return;

}