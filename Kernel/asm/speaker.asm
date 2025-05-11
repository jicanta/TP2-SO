GLOBAL outb
GLOBAL inb

section .text
inb:
    mov rdx, rdi
    mov rax, 0
    in al, dx
    ret

outb:
    mov rax, rsi
    mov rdx, rdi
    out dx, al
    ret