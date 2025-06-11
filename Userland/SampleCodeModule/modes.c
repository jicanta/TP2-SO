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
#include "include/memoryStructure.h"
#include "../tests/test.h"

#define EOF 0

// Variables internas del módulo
static int zoomAux, regAux;
static char *states[5] = {"Ready", "Running", "Blocked", "Dead", "Foreground"};

// Función auxiliar para imprimir con padding
void printWithPadding(const char* str, int width) {
    int len = strlen(str);
    print((char *)str);
    for (int i = len; i < width; i++) {
        print(" ");
    }
}

// Función auxiliar para imprimir número con padding
void printIntWithPadding(int num, int width) {
    char buffer[20];
    itoa(num, buffer);
    printWithPadding(buffer, width);
}

// =============================================================================
// HANDLERS DE COMANDOS CON NUEVA SIGNATURA
// =============================================================================

void handle_help(char* args) {
    printColor("Available commands:\n\n", CYAN);
    
    // Acceder a la tabla de comandos desde shell.c
    extern const Command command_table[];
    
    for (int i = 0; command_table[i].name != NULL; i++) {
        // Usar printf correctamente para el formateo
        printWithPadding(command_table[i].name, 20);
        print(" -> ");
        printWithPadding(command_table[i].description, 20);
        if (command_table[i].expected_args > 0) {
            printColor(" (requires arguments)", RED);
        }
        print("\n");
    }
    print("\n");
}

void handle_clear(char* args) {
    sysClearScreen();
}

void handle_time(char* args) {
    printTime();
}

void handle_date(char* args) {
    printDate();
}

void handle_zoomin(char* args) {
    zoomAux = incTextSize();
    if (zoomAux) {
        printColor("Maximum size reached.\n", YELLOW);
    } else {
        printColor("Text size increased.\n", GREEN);
    }
}

void handle_zoomout(char* args) {
    zoomAux = decTextSize();
    if (zoomAux) {
        printColor("Minimum size reached.\n", YELLOW);
    } else {
        printColor("Text size decreased.\n", GREEN);
    }
}

void handle_divbyzero(char* args) {
    printColor("Testing division by zero...\n", YELLOW);
    divZero();
}

void handle_invalidop(char* args) {
    printColor("Testing invalid opcode...\n", YELLOW);
    invalidOpcode();
}

void handle_registers(char* args) {
    regAux = sysPrintRegs();
    if (regAux) {
        printColor("You need to save registers first by pressing ALT\n", YELLOW);
    }
}

void handle_mem(char* args) {
    MemoryStatus status;
    sysGetMemStatus(&status);
    
    printColor("=== Memory Status ===\n", CYAN);
    print("Total: ");
    printInt(status.total);
    print(" bytes\n");
    print("Used: ");
    printInt(status.used);
    print(" bytes\n");
    print("Free: ");
    printInt(status.free);
    print(" bytes\n");
    print("Base address: 0x");
    print(hexToString((uint64_t)status.base));
    print("\n");
    print("End address: 0x");
    print(hexToString((uint64_t)status.end));
    print("\n");
}

void handle_ps(char* args) {
    PID pid;
    creationParameters params;
    params.name = "ps";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)ps_internal;
    params.foreground = 1;
    params.fds[0] = STDIN;
    params.fds[1] = STDOUT;
    pid = createProcess(&params);
    sysWait(pid, NULL);
}

void handle_loop(char* args) {
    PID pid;
    creationParameters params;
    params.name = "loop";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)printPidAndSayHi;
    params.foreground = 1;
    params.fds[0] = STDIN;
    params.fds[1] = STDOUT;
    pid = createProcess(&params);
    sysWait(pid, NULL);
}

void handle_kill(char* args[]) {
    
    if (!args || strlen(args) == 0) {
        printColor("Usage: kill [PID]\n", YELLOW);
        printColor("Example: kill 5\n", CYAN);
        return;
    }
    
    // Parsear PID

    int pid = atoi(args[0]);
    
    if (pid <= 0) {
        printColor("Error: Invalid PID. Must be a positive number.\n", RED);
        return;
    }
    
    // Validaciones de seguridad
    if (pid == 1) {
        printColor("Error: Cannot kill idle process (PID 1).\n", RED);
        return;
    }
    
    if (pid == 2) {
        printColor("Warning: Killing shell process. System will need restart.\n", YELLOW);
    }
    
    // Ejecutar kill
    int result = sysKill(pid);
    if (result == -1) {
        printColor("Error: Failed to kill process. PID may not exist.\n", RED);
    } else {
        printColor("Process killed successfully.\n", GREEN);
    }
}

void handle_nice(char* args) {
    if (!args || strlen(args) == 0) {
        printColor("Usage: nice [PID] [PRIORITY]\n", YELLOW);
        printColor("Example: nice 5 3\n", CYAN);
        return;
    }
    
    // Parsear argumentos
    char args_copy[256];
    strcpy(args_copy, args); // Copia para no modificar el original
    
    char* pid_str = strtok(args_copy, " ");
    char* priority_str = strtok(NULL, " ");
    
    if (!pid_str || !priority_str) {
        printColor("Error: Both PID and priority are required.\n", RED);
        printColor("Usage: nice [PID] [PRIORITY]\n", YELLOW);
        return;
    }
    
    int pid = atoi(pid_str);
    int priority = atoi(priority_str);
    
    // Validaciones
    if (pid <= 0) {
        printColor("Error: Invalid PID.\n", RED);
        return;
    }
    
    if (priority < 0 || priority > 10) {
        printColor("Error: Priority must be between 0 and 10.\n", RED);
        return;
    }
    
    // Ejecutar nice
    int result = sysNice(pid, priority);
    if (result == -1) {
        printColor("Error: Failed to change priority.\n", RED);
    } else {
        print("Priority of process ");
        printInt(pid);
        print(" changed to ");
        printInt(priority);
        printColor(" successfully.\n", GREEN);
    }
}

void handle_block(char* args) {
    if (!args || strlen(args) == 0) {
        printColor("Usage: block [PID]\n", YELLOW);
        printColor("Example: block 5\n", CYAN);
        return;
    }
    
    int pid = atoi(args);
    if (pid <= 0) {
        printColor("Error: Invalid PID.\n", RED);
        return;
    }
    
    int result = sysBlock(pid);
    if (result == -1) {
        printColor("Error: Failed to block process.\n", RED);
    } else {
        print("Process ");
        printInt(pid);
        printColor(" blocked successfully.\n", GREEN);
    }
}

void handle_yield(char* args) {
    printColor("Yielding CPU...\n", YELLOW);
    sysYield();
}

void handle_mm_test(char* args) {
    // Determinar tamaño de memoria para el test
    char* memory_size = "1000000"; // 1MB por defecto
    if (args && strlen(args) > 0) {
        memory_size = args;
    }
    
    printColor("Starting memory manager test...\n", YELLOW);
    print("Memory size: ");
    print(memory_size);
    print(" bytes\n");
    
    PID pid;
    creationParameters params;
    params.name = "test_mm";
    params.argc = 1;
    char* argv[] = {memory_size, NULL};
    params.argv = argv;
    params.priority = 1;
    params.entryPoint = (entryPoint)test_mm;
    params.foreground = 1;
    pid = createProcess(&params);
    sysWait(pid, NULL);
}

int64_t dummy(uint64_t argc, char *argv[])
{
    //printColor("Dummy process started. Type 'exit' to terminate.\n", YELLOW);

    char buffer[100] = {'\0'};
    while (scanf(buffer, 100) != EOF)
    {
        if (strcmp(buffer, "exit") == 0)
        {
            return 0;
        }
    }
    return 0;
}

void handle_pipes_test(void){

    int fds[2];

    if(sysCreatePipe(fds) == -1){
        printColor("Error creating pipe.\n", RED);
        return;
    }
    
    creationParameters params;
    params.name = "pipe_test";
    params.argc = 0;
    params.argv = NULL;
    params.priority = 1;
    params.entryPoint = (entryPoint)dummy;
    params.foreground = 1;
    params.fds[0] = STDIN; // Lectura
    params.fds[1] = fds[1]; // Escritura

    printf("Starting pipes test : %d \n :", fds[1]);
    

    PID pid1 = createProcess(&params);

    creationParameters params2;
    params2.name = "pipe_test";
    params2.argc = 0;
    params2.argv = NULL;
    params2.priority = 1;
    params2.entryPoint = (entryPoint)dummy  ;
    params2.foreground = 1;
    params2.fds[0] = fds[0]; // Lectura
    params2.fds[1] = STDOUT; // Escritura

    printf("Starting pipes test : %d \n :", fds[0]);


    PID pid2 = createProcess(&params2);

    sysWait(pid1, NULL);
    sysWait(pid2, NULL);
    printColor("Pipes test completed.\n", GREEN);
}


void handle_sync(char* args) {

    
    char* memory_size = "1000000";

    printf("%d", atoi(memory_size));
    printColor("Starting sync test...\n", YELLOW);
    print("Memory size: ");
    print(memory_size);
    print(" bytes\n");
    
    PID pid;
    creationParameters params;
    params.name = "test_sem";
    params.argc = 1;
    char* argv[] = {memory_size, NULL};
    params.argv = argv;
    params.priority = 1;
    params.entryPoint = (entryPoint)test_sem;
    params.foreground = 1;
    params.fds[0] = STDIN; // Lectura
    params.fds[1] = STDOUT; // Escritura
    pid = createProcess(&params);
    sysWait(pid, NULL);
}

void handle_easteregg(char* args) {
    printColor("🎉 ", YELLOW);
    printColor("I'm", RED);
    printColor(" an", YELLOW);
    printColor(" eas", GREEN);
    printColor("ter ", BLUE);
    printColor("egg", WHITE);
    printColor("! 🎉\n", YELLOW);

    sysHideCursor();
    sysPrintCursor();

    // Melodía mejorada
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
    printColor("Hope you enjoyed the music! 🎵\n", CYAN);
}

// =============================================================================
// FUNCIONES AUXILIARES
// =============================================================================

void ps_internal(void) {
    Process *processes = sysGetPS();
    
    printColor("=== Process Information ===\n", CYAN);
    printColor("PID  Name         PPID Prio FG State      StackBase    StackEnd\n", WHITE);
    printColor("---------------------------------------------------------------------\n", WHITE);
    
    for (int i = 0; processes[i].pid != -1; i++) {
        // PID (ancho 4)
        printIntWithPadding(processes[i].pid, 4);
        print(" ");
        
        // Name (ancho 12)
        printWithPadding(processes[i].name, 12);
        print(" ");
        
        // Parent PID (ancho 4)
        printIntWithPadding(processes[i].parentpid, 4);
        print(" ");
        
        // Priority (ancho 4)
        printIntWithPadding(processes[i].priority, 4);
        print(" ");
        
        // Foreground (ancho 2)
        printIntWithPadding(processes[i].foreground, 2);
        print(" ");
        
        // State (ancho 10)
        printWithPadding(states[processes[i].state], 10);
        print(" ");
        
        // Stack addresses
        print("0x");
        print(hexToString((uint64_t)processes[i].stackBase));
        print(" 0x");
        print(hexToString((uint64_t)processes[i].stackEnd));
        print("\n");
    }
    
    sysFreePS(processes);
}

void printPidAndSayHi(void) {
    int counter = 0;
    while (1) {
        counter++;
        print("[");
        printInt(counter);
        print("] Hello! My PID is ");
        printInt(sysGetPID());
        print("\n");
        sysSleep(2, 0);
    }
}

// Funciones USERLAND 

#define MAX_BUFFER 2048

int handle_cat(uint64_t argc, char *argv[]) {
    
    char c = '\0';
    int idx = 0;
    char buffer[MAX_BUFFER + 1] = {'\0'};

    while ((c = getchar()) != EOF)
    {
        putchar(c);
        if (idx < MAX_BUFFER)
        {
            buffer[idx++] = c;
        }
    }
    
    print(buffer);
    print("\n");

}

//Cuenta cant de linea de input

void handle_wc(){

    int counter = 0;

    unsigned char c = 0;

    while( (c = getchar()) != EOF)
    {
        if (c == '\n')
        {
            counter++;
        }
    }

    printf("Cantidad de lineas : %d\n",counter);
}

#define FILTER_BUFFER_SIZE 1024

void handle_filter(){

    char vocales [] = {'a', 'e', 'i', 'o', 'u',
                      'A', 'E', 'I', 'O', 'U'};

    char buffer[FILTER_BUFFER_SIZE] = {'\0'};

    int idx = 0;

    unsigned char c = 0;

    while( (c = getchar()) != EOF)
    {
        for(int i = 0; i<10;i++){
            if(c == vocales[i]){
                buffer[idx++] = c;
                break;
            }
        }
    }

    print("Filtered vowels: ");
    print(buffer);
    print("\n");
    
}

