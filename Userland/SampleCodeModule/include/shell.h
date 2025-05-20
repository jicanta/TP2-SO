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

#define CLEAR_MODE          9
#define REGISTERS_MODE      10
#define EASTEREGG_MODE      11
#define MEMSTATUS           12
#define PS                 13
#define LOOP               14
#define KILL               15
#define NICE               16
#define BLOCK              17

extern const char* modes[];

extern const char* helpText[];




int init();

#endif