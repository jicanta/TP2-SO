// Declaraciones de syscalls.asm del Userland
#ifndef SYSCALLS_H
#define SYSCALLS_H

#define SECONDS_ARG 0x00
#define MINUTES_ARG 0x02
#define HOURS_ARG 0x04
#define DAY_ARG 0x07
#define MONTH_ARG 0x08
#define YEAR_ARG 0x09

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
} screen_info;

typedef struct {
    uint32_t total;  
    uint32_t used;   
    uint32_t free;    
    void    *base;    
    void    *end;     
} MemoryStatus;


void _sys_write(int fd, char *buffer, int count, int Color);
int _sys_read(int fd, char * buffer, int count);
uint64_t _sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);
uint64_t _sys_clear_screen();
void _sys_reset_position();
void _sys_jump_line();
uint16_t _sys_get_screen_info(screen_info *info);
uint64_t _sys_sleep(uint64_t ms);
uint64_t _sys_draw_string(char * string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color);
void _sys_inc_scale();
void _sys_dec_scale();
void _sys_get_scale(uint8_t *scale);
uint64_t _sys_set_scale(uint8_t scale);
void * _sys_malloc(uint32_t size);
void _sys_free(void *memorySegment);
void _sys_mstatus(MemoryStatus * status);


#endif