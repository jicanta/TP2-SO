// Declaraciones de syscalls.asm del Userland
// Es el mismo archivo que en Kernel. Por ahi se puede hacer una carpeta compartida
#ifndef SYSCALLS_H
#define SYSCALLS_H

#define SECONDS_ARG 0x00
#define MINUTES_ARG 0x02
#define HOURS_ARG 0x04
#define DAY_ARG 0x07
#define MONTH_ARG 0x08
#define YEAR_ARG 0x09

/*
GLOBAL _sys_write,
GLOBAL _sys_read
GLOBAL _sys_drawRectangle
GLOBAL _sys_clearScreen
GLOBAL _sys_reset_position
GLOBAL _sys_jump_line*/

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
} screen_info;

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
//uint64_t sys_get_time(time_struct * time)


#endif