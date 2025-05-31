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
void handle_kill(char* args);
void handle_nice(char* args);
void handle_block(char* args);
void handle_yield(char* args);
void handle_test(char* args);
void handle_easteregg(char* args);

// Funciones auxiliares
void ps_internal(void);
void printPidAndSayHi(void);

#endif // MODES_H