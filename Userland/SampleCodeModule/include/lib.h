#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <syscallsDispatchers.h>
#include <iolib.h>


typedef struct time {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
} time_struct;

uint64_t lib_set_scale(uint8_t scale);
uint8_t lib_get_scale();
void lib_get_time(time_struct *time);
uint64_t lib_read_char(char *buffer);
void lib_sleep(uint64_t millis);
void lib_draw_string(char *string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color);
void lib_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);

#endif