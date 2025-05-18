; filepath: /home/ian/SO/TP2-SO/Kernel/asm/contextSwitch.asm
global contextSwitch

section .text
contextSwitch:
    ; Guardar registros no volátiles
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov [rdi], rsp    ; Guarda el stack actual en *old_rsp
    mov rsp, rsi      ; Carga el stack del próximo proceso

    ; Restaurar registros no volátiles
    pop r15
    pop r14 
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret