// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/shell.h"
#include "include/modes.h"
#include "include/colors.h"
#include "include/syscalls.h"
#include "include/stdio.h"
#include "include/string.h"
#include "include/philosophers.h"
#include "../tests/test.h"

#define MAX_COMMANDS 2
#define MAX_ARGS 3


Command * commands[MAX_COMMANDS] = {NULL};

char *args[MAX_COMMANDS][MAX_ARGS] = {NULL};

int fds[MAX_COMMANDS][2];

int isForeground = 1;

int commandCount = 0;

int pipeFlag = 0;

const Command command_table[] = {
    {"help", (CommandFunc) handle_help, "Shows command descriptions", 0,1},
    {"clear", (CommandFunc)handle_clear, "Clears the screen", 0,1},
    {"time", (CommandFunc)handle_time, "Shows current time", 0,1},
    {"date", (CommandFunc)handle_date, "Shows current date", 0,1},
    {"cat", (CommandFunc)handle_cat, "Reads from stdin and outputs to stdout", 0,0},
    {"wc", (CommandFunc)handle_wc, "Counts lines, words, and characters from input", 0,0},
    {"filter", (CommandFunc)handle_filter, "Filters vowels from input text", 0,0},
   // {"phylo", handle_phylo, "Runs dining philosophers simulation", 0,1},
    
    {"mem", (CommandFunc)handle_mem, "Shows memory status", 0,1},
    {"ps", (CommandFunc)ps_internal, "Shows process information", 0,1},
    {"yield", (CommandFunc)handle_yield, "Yields CPU to other processes", 0,1},
    {"loop",(CommandFunc) printPidAndSayHi, "Starts infinite loop process", 0,0},
    {"kill", (CommandFunc)handle_kill, "[pid] Kills a process by PID", 1,1},
    {"nice",(CommandFunc) handle_nice, "[pid] [new priority] Changes process priority", 2,1},
    {"block",(CommandFunc) handle_block, "[pid] Blocks a process by PID", 1,1},
    {"philo",(CommandFunc) startPhilo, "Example philosophers program", 0,0},
    {"testmm",(CommandFunc) test_mm, "[max memory] Runs memory manager test", 1,0},
    {"testproc",(CommandFunc) test_processes, "[max processes] Runs memory manager test", 1,0},
    {"testprio",(CommandFunc) test_prio, "Runs memory manager test", 0,0},
    {"testsynchro",(CommandFunc) test_sync, "[initial count] [use semaphores] Runs memory manager test", 2,0},
  
  
    {NULL, NULL, NULL, 0} // Terminador
};





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

    PID pid[MAX_COMMANDS];

    for(int i = 0; i <= commandCount; i++) {

        const Command * cmd = commands[i];

        if (cmd != NULL) {
            if (cmd->isBuiltIn && !pipeFlag) {
                if(!isForeground){
                    printColor("Background commands cannot be built-in.\n", RED);
                    closeFDsMadeByParser();
                    return -1;
                }

                if(cmd->expected_args == 0 && args[i][0] != NULL) {
                    printColor("This comand does not expect arguments.\n", RED);
                    closeFDsMadeByParser();
                    return -1;
                }
                
                cmd->handler(args[i]); //Comando
            } else {

                int argc = 0;
                for(int j = 0; j < MAX_ARGS && args[i][j] != NULL; j++) {
                    argc++;
                }
                
                // Comando externo
                creationParameters params;
                params.name = (char*)cmd->name;
                params.argc = argc;
                params.argv = args[i];
                params.priority = 1;
                params.entryPoint = (entryPoint)cmd->handler;
                params.foreground = isForeground;
                params.fds[0] = fds[i][0]; // Lectura
                params.fds[1] = fds[i][1]; // Escritura
                
                pid[i] = createProcess(&params);

            }
        } else {
            print_command_not_found(commands[i]->name);
        }
    }

    if(isForeground) {
        for(int i = 0; i <= commandCount; i++) {
            sysWait(pid[i], NULL);
        }
    }
    
    return 0;
}

int parseConsolePrompt(char* input){

    isForeground = 1; // Default to foreground execution
    pipeFlag = 0; // Reset pipe flag
    commandCount = 0; // Reset command count

    initializeFd(); // Reset file descriptors

    char * token = strtok(input, " ");

    while(token != NULL){

        commands[commandCount] = (Command*)find_command(token); // Find command in the table

        if (commands[commandCount] == NULL) {

            print_command_not_found(token);
            closeFDsMadeByParser(); 
            return -1; 

        }     
        
        token = strtok(NULL, " ");

        int argsc = 0;

        while(token != NULL && strcmp(token, "|") != 0 && strcmp(token, "&") != 0) {

            if (argsc >= MAX_ARGS) {
                closeFDsMadeByParser(); 
                return -1;
            }

            args[commandCount][argsc++] = token; // Store the argument
            token = strtok(NULL, " ");
        }

        if(token != NULL && strcmp(token, "&") == 0){
            
            if(pipeFlag){
                printColor("No background commands allowed in pipes.\n", RED);
                closeFDsMadeByParser();
                return -1;
            }

            isForeground = 0; // Set background execution

            token = strtok(NULL, " "); // Get next token after '&'

        }

        if(token != NULL && strcmp(token, "|") == 0) {

            if(!isForeground){
                printColor("Background commands cannot be piped.\n", RED);
                closeFDsMadeByParser();
                return -1;
            }
            // Handle pipe
            if(pipeFlag){
                printColor("More than one pipe is not allowed in single command execution.\n", RED);
                closeFDsMadeByParser();
                return -1;
            }

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

int closeFDsMadeByParser(){

        for(int i = 0; i < MAX_COMMANDS; i++) {
            sysCloseFD(fds[i][0]); // Close read end
            sysCloseFD(fds[i][1]); // Close write end
        }
        return 0;
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