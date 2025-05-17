#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
/*



    EN LOS .C USAR LAS DE ASM (QUE ARRANCAN CON '_') !!!!!!!!!!!!!!!11



*/



#define STDOUT 1
#define STDERR 2

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
} Registers;

void syscall_handler(Registers *regs);

void sys_write(int fd, char *buffer, int count);

int sys_read(int fd, char * buffer, int count);

void sys_reset_position();

#endif