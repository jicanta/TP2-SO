#ifndef __SHELL_H_
#define __SHELL_H_

/**
 * Initializes Shell 
*/

#define IDLE_MODE           1
#define MAX_COMMAND_LENGTH  32

typedef void (*CommandFunc)(void* args);

typedef struct {
    const char* name;
    CommandFunc handler; // Función que maneja el comando
    const char* description;
    int expected_args; // Indica si el comando espera argumentos
    int isBuiltIn; // Indica si es un comando interno
} Command;

// Funciones principales de la shell
int init(void);
void print_command_not_found(const char* command);
int execute_commands();

// Funciones auxiliares
char* parse_command(char* input, char** args);
const Command* find_command(const char* name);
void trim_whitespace(char* str);

// Tabla de comandos externa (definida en shell.c)
extern const Command command_table[];

int parseConsolePrompt(char* input);
void initializeFd(void);
int closeFDsMadeByParser(void);

#endif