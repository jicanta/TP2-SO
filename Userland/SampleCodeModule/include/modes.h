#ifndef MODES_H
#define MODES_H

#include "processStructure.h"

// Handlers de comandos (nueva signatura con char* args)
void handle_help(char* args);
void handle_clear(char* args);
void handle_time(char* args);
void handle_date(char* args);
void handle_zoomin(char* args);
void handle_zoomout(char* args);
void handle_divbyzero(char* args);
void handle_invalidop(char* args);
void handle_registers(char* args);
void handle_mem(char* args);
void handle_ps(char* args);
void handle_loop(char* args);
void handle_kill(char* args[]);
void handle_nice(char* args[]);
void handle_block(char* args[]);
void handle_yield(char* args);
void handle_mm_test(char* args);
void handle_pipes_test(void);
void handle_easteregg(char* args);
void handle_sync(char* args);

int handle_cat(uint64_t argc, char *argv[]);
void handle_wc();
void handle_filter();

void printFD();
void printSem();

// Funciones auxiliares
void ps_internal(void);
void printPidAndSayHi(void);


#endif // MODES_H