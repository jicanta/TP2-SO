#ifndef TPE_ARQ_SHELL_H
#define TPE_ARQ_SHELL_H

<<<<<<< HEAD
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
#define MEMSTATE_MODE       13
=======
#include <iolib.h>
#include <color.h>
#include <commands.h>
#include <stringutils.h>
#include <syscallsDispatchers.h>
>>>>>>> parent of 7a25ff6 ([IMPORTANT] change kernel)

#define UP_ARROW 0x41
#define BACKSPACE 0x08
#define ESC 27
#define BUFFER_SIZE 256

<<<<<<< HEAD
static const char* modes[] = {
    "shell", "idle", "help", "divbyzero", "invalidopcode", "zoomin", "zoomout", "time", "date", "eliminator", "clear", "registers", "easteregg", "memstate"
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
"MEMSTATE            ->      Shows memory state.\n",
"end" 
};

int init();
=======
typedef struct buffer {
    char items[BUFFER_SIZE];
    int size;
} TBuffer;

/**
 * @brief Starts the shell.
 */
void shell();
>>>>>>> parent of 7a25ff6 ([IMPORTANT] change kernel)

#endif //TPE_ARQ_SHELL_H