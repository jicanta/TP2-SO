#include "include/shell.h"
#include "include/modes.h"
#include "include/colors.h"
#include "include/syscalls.h"
#include "include/stdio.h"
#include "include/string.h"
#include "../tests/test.h"

#define MAX_COMMANDS 2
#define MAX_ARGS 3


char * commands[MAX_COMMANDS] = {NULL};

char *args[MAX_COMMANDS][MAX_ARGS] = {NULL};

int fds[MAX_COMMANDS][2];

int commandCount = 0;

 int pipeFlag = 0;


const Command command_table[] = {
    {"help", handle_help, "Shows command descriptions", 0,0},
    {"clear", handle_clear, "Clears the screen", 0,0},
    {"time", handle_time, "Shows current time", 0,0},
    {"date", handle_date, "Shows current date", 0,0},
    {"cat", handle_cat, "Reads from stdin and outputs to stdout", 0,0},
    {"wc", handle_wc, "Counts lines, words, and characters from input", 0,0},
    {"filter", handle_filter, "Filters vowels from input text", 0,0},
   // {"phylo", handle_phylo, "Runs dining philosophers simulation", 0,1},
    
   // {"easteregg", handle_easteregg, "Easter egg surprise", 0,0},
   // {"zoomin", handle_zoomin, "Enlarges text size", 0,0},
   //{"zoomout", handle_zoomout, "Reduces text size",0,0},
   // {"divbyzero", handle_divbyzero, "Tests division by zero", 0,0},
   // {"invalidopcode", handle_invalidop, "Tests invalid opcode", 0},
    {"registers", handle_registers, "Shows register values", 0,1},
    {"mem", handle_mem, "Shows memory status", 0,1},
    {"ps", ps_internal, "Shows process information", 0,0},
    {"yield", handle_yield, "Yields CPU to other processes", 0,1},
    {"loop", handle_loop, "Starts infinite loop process", 0,1},
    {"kill", handle_kill, "Kills a process by PID", 1,1},
    {"nice", handle_nice, "Changes process priority", 2,1},
    {"block", handle_block, "Blocks a process by PID", 1,1},
    //TODO : ca hay q ver q hacemos con estos
    {"test_mm", test_mm, "Runs memory manager test", 0,1},
    {"test_pipes", handle_pipes_test, "Runs pipes test", 0,1},
    {"syncro", test_sem, "Runs memory semaphore test", 0,1},
    {"printfd", printFD, "Prints file descriptors", 0,0},
    {"printsem", printSem, "Prints semaphores", 0,0},
  
    {NULL, NULL, NULL, 0} // Terminador
};

void printFD(){
    sysPrintFD();
}

void printSem(){
    sysPrintSem();
}


// TO DO : ESTO ESTA MAL, CADA COMANDO ES UN PROCESO NUEVO

void initializeFd(){
    for(int i = 0; i < MAX_COMMANDS; i++) {
        fds[i][0] = STDIN; // Lectura
        fds[i][1] = STDOUT; // Escritura
    }
}

int init() {

    initializeFd();
    
    printColor("=== Welcome to Shell! ===\n", CYAN);
    printColor("Type 'help' for command information.\n\n", YELLOW);
    
    char commandPrompt[MAX_COMMAND_LENGTH] = {0};
    
    while (IDLE_MODE) {
        // Limpiar buffer y mostrar prompt
        sysClearKbEntry();
        printColor("$ ", GREEN);
        print("> ");
        sysShowCursor();
        
        // Leer comando
        if (scanf(commandPrompt, MAX_COMMAND_LENGTH) <= 0) {
            continue;
        }
        
        // Procesar comando
        if (strlen(commandPrompt) == 0) {
            continue; // Entrada vacía
        }
        
        parseConsolePrompt(commandPrompt);
                
        sysShowCursor();


    }
    return 0;
}


int execute_commands() {
        //printColor("Executing command: ", YELLOW);
    PID pid[MAX_COMMANDS];

    for(int i = 0; i <= commandCount; i++) {


        const Command* cmd = find_command(commands[i]);

        if (cmd != NULL) {
            if (!cmd->isBuiltIn && !pipeFlag) {
                // Comando interno
                cmd->handler(args[i][0]);
            } else {

                int argc = 0;
                for(int j = 0; j < MAX_ARGS && args[i][j] != NULL; j++) {
                    argc++;
                }
                // Comando externo
                creationParameters params;
                params.name = commands[i];
                params.argc = argc;
                params.argv = args[i];
                params.priority = 1;
                params.entryPoint = (entryPoint)cmd->handler;
                params.foreground = 1;
                params.fds[0] = fds[i][0]; // Lectura
                params.fds[1] = fds[i][1]; // Escritura
                
                pid[i] = createProcess(&params);

            }
        } else {
            print_command_not_found(commands[i]);
        }
    }

    for(int i = 0; i < commandCount; i++) {
        // Close write end of pipes
        sysWait(pid[i], NULL);
    }

    return 0;
}



int parseConsolePrompt(char* input){

    pipeFlag = 0; // Reset pipe flag
    commandCount = 0; // Reset command count

    for(int i =0;i<MAX_COMMANDS;i++){
        fds[i][0] = STDIN; // Reset read end
        fds[i][1] = STDOUT; // Reset write end
    }

    char * token = strtok(input, " ");


    while(token != NULL){

        commands[commandCount] = token; 
        
        token = strtok(NULL, " ");

        int argsc = 0;

        while(token != NULL && strcmp(token, "|") != 0 && strcmp(token, "&") != 0) {

            if (argsc >= MAX_ARGS) {
                // Exceeded maximum number of arguments
                return -1;
            }

            args[commandCount][argsc++] = token; // Store the argument

            token = strtok(NULL, " ");
        }

        if(token != NULL && strcmp(token, "|") == 0) {
            // Handle pipe
            int pipe[2];
            sysCreatePipe(pipe);
            fds[commandCount][1] = pipe[1]; //El primero lee de stdin y escribe en el pipe
            fds[commandCount+1][0] = pipe[0]; // EL segundo lee de pipe y escribe en stdout
            commandCount++; // Increment command count for next command
            token = strtok(NULL, " "); // Get next token after pipe
            pipeFlag = 1; // Set pipe flag

        }

    }

    return execute_commands();

}

// Buscar comando en la tabla
const Command* find_command(const char* name) {
    for (int i = 0; command_table[i].name != NULL; i++) {
        if (strcasecmp(name, command_table[i].name) == 0) {
            return &command_table[i];
        }
    }
    return NULL;
}

// Parsear comando y argumentos (sin trim de espacios)
char* parse_command(char* input, char** args) {
    char* command = strtok(input, " ");
    *args = strtok(NULL, ""); // Resto como argumentos
    return command;
}

// Función para comando no encontrado
void print_command_not_found(const char* command) {
    printColor("Command '", RED);
    printColor(command, WHITE);
    printColor("' not found. Type 'help' for available commands.\n", RED);
}