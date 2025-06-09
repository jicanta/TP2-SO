#ifndef __SHELL_H_
#define __SHELL_H_

/**
 * Initializes Shell 
*/

#define IDLE_MODE           1
#define MAX_COMMAND_LENGTH  32

typedef struct {
    const char* name;
    void (*handler)(char* args);
    const char* description;
    int expected_args; // Indica si el comando espera argumentos
    int isBuiltIn; // Indica si es un comando interno
} Command;

// Funciones principales de la shell
int init(void);
int execute_command(char* input);
void print_command_not_found(const char* command);
int execute_commands();

// Funciones auxiliares
char* parse_command(char* input, char** args);
const Command* find_command(const char* name);
void trim_whitespace(char* str);

// Tabla de comandos externa (definida en shell.c)
extern const Command command_table[];

#endif