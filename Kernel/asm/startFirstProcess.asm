; filepath: /home/ian/SO/TP2-SO/Kernel/asm/startFirstProcess.asm
global startFirstProcess

section .text
startFirstProcess:
    
    mov rsp, rdi
 
    pop rax        ; Dirección de la función (idleProcess)
    call rax

    ret

