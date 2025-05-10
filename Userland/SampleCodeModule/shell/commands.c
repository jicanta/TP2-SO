#include <iolib.h> 
#include <commands.h>
#include "time.h"
#include <stringutils.h>
#include <memlib.h>

#define COMMANDS_QUANTITY 10

static void toUtcMinus3(time_struct * time);

COMMAND commands[COMMANDS_QUANTITY] = {
    {"help", "Muestra todos los comandos disponibles", help_function},
    {"clear", "Limpia la pantalla", clear_function},
    {"registers", "Muestra los valores actuales de los registros", registers_function},
    {"time", "Muestra la hora actual", time_function},
    {"che0","Corre programa para chequear excepciones", check_0_div},
    {"che1","Corre programa para chequear excepciones", check_no_opcode},
    {"zoom in", "Incrementa la escala", zoomin_function},
    {"zoom out", "Decrementa la escala", zoomout_function},
    {"memstatus", "Muestra estado de la memoria", memory_status_function},
    {"malloc", "Asigna memoria", malloc_function},
};

// Implementations

void clear_function() {
    clearScreen();
}

void registers_function() {
    _sys_get_registers();
}

void time_function(int argc, char * argv[]) {
    time_struct time;
    _sys_get_time(&time);
    toUtcMinus3(&time);
    printf("%d/%d/%d [d/m/y]\n", time.day, time.month, time.year);
    int64_t h = time.hour;
    printf("%d:%d:%d [hour/min/sec] (Argentina)\n", h, time.minutes, time.seconds);  // la hora es -3 para que este en tiempo argentino.
    return;
}

static void toUtcMinus3(time_struct * time) {

    if (time->hour < 3) {
        time->hour += 21;
        time->day--;
        if (time->day == 0) {
            time->month--;
            if (time->month == 0) {
                time->month = 12;
                time->year--;
            }
            if(time->month == 2){
                time->day = 28;
                if(time->year % 4 == 0){
                    time->day = 29;
                }
            } else if(time->month == 4 || time->month == 6 || time->month == 9 || time->month == 11){
                time->day = 30;
            } else {
                time->day = 31;
            }
        }
    }
    else{
        time->hour = time->hour - 3;
    }
}

void check_0_div() {
    int a = 0;
    int b = 1;
    int c = b / a;
    return;
}

void check_no_opcode(){
    invalid_opcode();
}

void help_function(){
    printf("Los comandos disponibles son:");
    println();

    for(int i = 0; i < COMMANDS_QUANTITY;i++){
        printf(commands[i].name);
        printf(" - ");
        printf(commands[i].description);
        println();
    }
}

void execute(char *name){
    println();
    for(int i = 0; i < COMMANDS_QUANTITY; i++){
        if(strcmp(commands[i].name, without_final_spaces(name)) == 0){
            commands[i].function();
            return;
        }
    }
    printf("Comando '%s' no encontrado.\n", name);
    return;
}

void zoomin_function(){
    _sys_inc_scale();
}

void zoomout_function(){
    _sys_dec_scale();
}

char * fillCommand(char * buffer, unsigned int len){
    
    for (int i = 0; (i < COMMANDS_QUANTITY); i++){
        if (strncmp(buffer, commands[i].name, len) == 0){
            return commands[i].name;
        }
    }
    return NULL;
}

void malloc_function(){
    void *ptr = malloc(16);
    
    if(ptr == NULL){
        printf("Error al asignar memoria\n");
        return;
    }
    printf("Direccion de memoria: %x\n", ptr);
    //_sys_free(ptr);
    //printf("Memoria liberada correctamente\n");
    return;
}

void memory_status_function(){
    MemoryStatus status;
    _sys_mstatus(&status);
    printf("Total: %d bytes\n", status.total);
    printf("Usados: %d bytes\n", status.used);
    printf("Libres: %d bytes\n", status.free);
    printf("Base: %x\n", status.base);
    printf("Fin: %x\n", status.end);
    return;
}