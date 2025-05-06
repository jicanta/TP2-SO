#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <lib.h>

#define SNAPSHOT_REGS_LENGTH 17

#define STDIN 0
#define STDOUT 1

#define START_X 0
#define START_Y 0

#define MARGIN 20
#define LINE_SPACE 4
#define SPACE_BETWEEN 2

#define REGISTERS_AMOUNT 15
#define MAX_INT_DIGITS 20

#define STDOUT 1
#define STDERR 2

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax, rsp, rip;
} Registers;

typedef struct {
    uint16_t width;
    uint16_t height;
} screen_info;

typedef struct time {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
} time_struct;

void writeNumber(uint64_t num, char * str);

void syscall_handler(Registers *regs);

void sys_write(int fd, char *buffer, int count, int Color);

int sys_read(int fd, char * buffer, int count);

uint64_t sys_draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);

uint64_t sys_draw_string(char * string, uint64_t x, uint64_t y, uint32_t color, uint32_t bg_color);

uint64_t sys_clear_screen();

void sys_reset_position();

void sys_jump_line();

uint64_t sys_sleep(uint64_t ticks);

void sys_get_screen_info(screen_info *info);

void sys_get_registers();

uint64_t sys_get_time(time_struct * time); 

void sys_inc_scale();

void sys_dec_scale();

void sys_get_scale(uint8_t *scale);

uint64_t sys_set_scale(uint8_t scale);

void * sys_malloc(uint32_t size);

void sys_free(void *memorySegment);

void sys_mstatus(uint32_t *status);


#endif