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


void dispatcherWrite(int fd, char *buffer, int count, int Color);
int dispatcherRead(int fd, char * buffer, int count);
uint64_t dispatcherDrawRectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);
uint64_t dispatcherClearScreen();
void dispatcherResetPosition();
void dispatcherJumpLine();
uint16_t dispatcherGetScreenInfo(screen_info *info);
uint64_t dispatcherSleep(uint64_t ms);
uint64_t dispatcherDrawString(char * string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color);
void dispatcherIncScale();
void dispatcherDecScale();
void dispatcherGetScale(uint8_t *scale);
uint64_t dispatcherSetScale(uint8_t scale);
void * dispatcherMalloc(uint32_t size);
void dispatcherFree(void *memorySegment);
void dispatcherMemStatus(MemoryStatus * status);


#endif