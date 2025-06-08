#include "include/shell.h"
#include "include/modes.h"
#include "include/colors.h"
#include "include/syscalls.h"
#include "include/stdio.h"
#include "include/string.h"

#define MAX_COMMANDS 2
#define MAX_ARGS 3

const Command command_table[] = {
    {"help", handle_help, "Shows command descriptions", 0,0},
    {"clear", handle_clear, "Clears the screen", 0,0},
    {"time", handle_time, "Shows current time", 0,0},
    {"date", handle_date, "Shows current date", 0,0},
   // {"easteregg", handle_easteregg, "Easter egg surprise", 0,0},
   // {"zoomin", handle_zoomin, "Enlarges text size", 0,0},
   //{"zoomout", handle_zoomout, "Reduces text size",0,0},
   // {"divbyzero", handle_divbyzero, "Tests division by zero", 0,0},
   // {"invalidopcode", handle_invalidop, "Tests invalid opcode", 0},
    {"registers", handle_registers, "Shows register values", 0,1},
    {"mem", handle_mem, "Shows memory status", 0,1},
    {"ps", handle_ps, "Shows process information", 0,1},
    {"yield", handle_yield, "Yields CPU to other processes", 0,1},
    {"loop", handle_loop, "Starts infinite loop process", 0,1},
    {"kill", handle_kill, "Kills a process by PID", 1,1},
    {"nice", handle_nice, "Changes process priority", 2,1},
    {"block", handle_block, "Blocks a process by PID", 1,1},
    {"test_mm", handle_mm_test, "Runs memory manager test", 0,0},
    {"test_pipes", handle_pipes_test, "Runs pipes test", 0,0},
    {"syncro", handle_sync, "Runs memory semaphore test", 0,0},
    {NULL, NULL, NULL, 0} // Terminador
};


// TO DO : ESTO ESTA MAL, CADA COMANDO ES UN PROCESO NUEVO


int init() {
    
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
        
        if (!execute_command(commandPrompt)) {
            print_command_not_found(commandPrompt);
        }
    }
    return 0;
}

char * commands[MAX_COMMANDS] = {NULL};

char *args[MAX_COMMANDS][MAX_ARGS] = {NULL};

int fds[MAX_COMMANDS][2];


int execute_command(char* input) {

    if (!input || strlen(input) == 0) {
        return 0;
    }

    char * token;
    int commandsCount=0;
    int argsc = 0;

    token = strtok(input, " ");

    if (token == NULL) {
        return 0; // No command entered
    }

    commands[commandsCount] = token; // Store the command

    for(int i =0; i < MAX_COMMANDS; i++) {
        fds[i][0] = STDIN; // Reset file descriptors
        fds[i][1] = STDOUT;
    }
    
    while(token != NULL){

        while (token != NULL && strcmp(token, "|") != 0 && strcmp(token, "&") != 0)
        {
            if (argsc >= MAX_ARGS)
            {
                //closeFDsmadeByParser();
               // perror("Error: You exceeded the maximum number of arguments\n");
                return;
            }
            args[commandsCount][argsc++] = token;
            
            token = strtok(NULL, " ");
        }
        //if()
    }

    Command* command = find_command(commands[commandsCount]);

    if(command == NULL){
        return 0;
    }

    command->handler(args[commandsCount]);

    return 1;
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