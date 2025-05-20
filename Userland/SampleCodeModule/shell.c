#include "include/shell.h"
#include "include/modes.h"
#include "include/colors.h"
#include "include/syscalls.h"
#include "include/stdio.h"
#include "include/string.h"

const char* modes[] = {
    "shell", "idle", "help", "divbyzero", "invalidopcode", "zoomin", "zoomout", "time", "date",  "clear", "registers", "easteregg","memstatus", "ps", "loop", "kill", "nice", "block"
};

const char* helpText[] = { "Command information is displayed below:\n\n",
"HELP                ->      Shows a description on each available command.\n",
"DIVBYZERO           ->      Shows handling in case of division by zero.\n",
"INVALIDOPCODE       ->      Shows handling in case of an invalid operation code.\n",
"ZOOMIN              ->      Enlarges text size on screen. In case maximum size is reached,\n",
"                            it is properly indicated without making any changes.\n",
"                            Can be alternatively activated with CTRL + i\n",
"ZOOMOUT             ->      Reduces text size on screen. In case minimum size is reached,\n",
"                            it is properly indicated without making any changes.\n", 
"                            Can be alternatively activated with CTRL + o\n",
"TIME                ->      Shows current time in HH:MM:SS format.\n",
"DATE                ->      Shows current date in DD/MM/YY format.\n",
"CLEAR               ->      Clears the screen\n",
"REGISTERS           ->      Prints registers values. To do this, first you need to save\n",
"                            your registers by pressing ALT.\n",
"MEMSTATUS           ->      Prints memory status.\n",
"PS                  ->      Prints processes information.\n",
"LOOP                ->      Prints actual PID and welcome.\n",
"KILL [PID]          ->     Kills a process given a PID.\n",
"NICE [PID] [PRIORITY] ->   Changes the priority of a process given a PID and a new priority.\n",
"BLOCK [PID]           ->   Blocks a process given a PID.\n",
"end" 
};

int init(){
    printColor("Welcome to Shell! Type HELP for command information.\n\n", YELLOW);
    char commandPrompt[32]={0};
    while(IDLE_MODE) {
        sysClearKbEntry();
        printColor("$", GREEN);
        print("> ");
        sysShowCursor();
        scanf(commandPrompt, 32);
        if(strcasecmp(commandPrompt, modes[HELP_MODE]) == SELECTED_MODE) help();
        else if(strcasecmp(commandPrompt, modes[CLEAR_MODE]) == SELECTED_MODE) clear();
        else if(strcasecmp(commandPrompt, modes[TIME_MODE]) == SELECTED_MODE) time();
        else if(strcasecmp(commandPrompt, modes[DATE_MODE]) == SELECTED_MODE) date();
        else if(strcasecmp(commandPrompt, modes[EASTEREGG_MODE]) == SELECTED_MODE) playEasterEgg();
        else if(strcasecmp(commandPrompt, modes[ZOOMIN_MODE]) == SELECTED_MODE ) zoomin();
        else if(strcasecmp(commandPrompt, modes[ZOOMOUT_MODE]) == SELECTED_MODE) zoomout();
        else if(strcasecmp(commandPrompt, modes[DIVBYZERO_MODE]) == SELECTED_MODE) divByZero();
        else if(strcasecmp(commandPrompt, modes[INVALIDOPCODE_MODE]) == SELECTED_MODE) invalidOp();
        else if(strcasecmp(commandPrompt, modes[REGISTERS_MODE]) == SELECTED_MODE) registers();
        else if(strcasecmp(commandPrompt, modes[MEMSTATUS]) == SELECTED_MODE) printMemoryStatus();
        else if(strcasecmp(commandPrompt, modes[PS]) == SELECTED_MODE) printProcessesInformation();
        else if(strcasecmp(commandPrompt, modes[LOOP]) == SELECTED_MODE) loop();
        else if (strncasecmp(commandPrompt, modes[KILL], strlen(modes[KILL])) == 0) {

            char *token = strtok(commandPrompt, " "); // Divide el comando por espacios
            token = strtok(NULL, " "); // Obtén el siguiente token (el argumento)
            if (token != NULL) {
                int pid = atoi(token); // Convierte el argumento a un entero
                kill(pid); // Llama a la función kill con el PID
            } else {
                printColor("Error: Missing argument for kill command.\n", RED);
            }
        }
        
        else if (strncasecmp(commandPrompt, modes[NICE], strlen(modes[NICE])) == 0) {

            char *token = strtok(commandPrompt, " "); // Divide el comando por espacios
            token = strtok(NULL, " "); // Obtén el siguiente token (el argumento)
            char * token2 = strtok(NULL, " "); // Obtén el siguiente token (el argumento)
            if (token != NULL && token2 != NULL) {
                int pid = atoi(token); // Convierte el argumento a un entero
                int priority = atoi(token2); // Convierte el argumento a un entero
                nice(pid,priority); // Llama a la función kill con el PID
            } else {
                printColor("Error: Missing argument for nice command.\n", RED);
            }
        }
         else if (strncasecmp(commandPrompt, modes[BLOCK], strlen(modes[BLOCK])) == 0) {

            char *token = strtok(commandPrompt, " "); // Divide el comando por espacios
            token = strtok(NULL, " "); // Obtén el siguiente token (el argumento)
            if (token != NULL) {
                int pid = atoi(token); // Convierte el argumento a un entero
                block(pid); // Llama a la función kill con el PID
            } else {
                printColor("Error: Missing argument for block command.\n", RED);
            }
        }
        
        else notFound(commandPrompt);
    }
}