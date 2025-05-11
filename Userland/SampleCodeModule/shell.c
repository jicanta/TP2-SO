#include "include/shell.h"
#include "include/modes.h"
#include "include/colors.h"
#include "include/syscalls.h"
#include "include/stdio.h"
#include "include/string.h"

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
        else if(strcasecmp(commandPrompt, modes[ELIMINATOR_MODE]) == SELECTED_MODE) eliminator();
        else if(strcasecmp(commandPrompt, modes[CLEAR_MODE]) == SELECTED_MODE) clear();
        else if(strcasecmp(commandPrompt, modes[TIME_MODE]) == SELECTED_MODE) time();
        else if(strcasecmp(commandPrompt, modes[DATE_MODE]) == SELECTED_MODE) date();
        else if(strcasecmp(commandPrompt, modes[EASTEREGG_MODE]) == SELECTED_MODE) playEasterEgg();
        else if(strcasecmp(commandPrompt, modes[ZOOMIN_MODE]) == SELECTED_MODE ) zoomin();
        else if(strcasecmp(commandPrompt, modes[ZOOMOUT_MODE]) == SELECTED_MODE) zoomout();
        else if(strcasecmp(commandPrompt, modes[DIVBYZERO_MODE]) == SELECTED_MODE) divByZero();
        else if(strcasecmp(commandPrompt, modes[INVALIDOPCODE_MODE]) == SELECTED_MODE) invalidOp();
        else if(strcasecmp(commandPrompt, modes[REGISTERS_MODE]) == SELECTED_MODE) registers();
        else notFound(commandPrompt);
    }
}