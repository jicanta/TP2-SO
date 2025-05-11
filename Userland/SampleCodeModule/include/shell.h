#ifndef __SHELL_H_
#define __SHELL_H_

/**
 * Initializes Shell 
*/

#define SELECTED_MODE       0

#define IDLE_MODE           1
#define HELP_MODE           2
#define DIVBYZERO_MODE      3
#define INVALIDOPCODE_MODE  4
#define ZOOMIN_MODE         5
#define ZOOMOUT_MODE        6
#define TIME_MODE           7
#define DATE_MODE           8
#define ELIMINATOR_MODE     9
#define CLEAR_MODE          10
#define REGISTERS_MODE      11
#define EASTEREGG_MODE      12


static const char* modes[] = {
    "shell", "idle", "help", "divbyzero", "invalidopcode", "zoomin", "zoomout", "time", "date", "eliminator", "clear", "registers", "easteregg"
};

static char* helpText[] = { "Command information is displayed below:\n\n",
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
"ELIMINATOR          ->      Opens ELIMINATOR game.\n",
"CLEAR               ->      Clears the screen\n",
"REGISTERS           ->      Prints registers values. To do this, first you need to save\n",
"                            your registers by pressing ALT.\n",
"end" 
};

int init();

#endif