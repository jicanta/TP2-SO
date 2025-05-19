GLOBAL divZero
GLOBAL invalidOpcode

section .text

divZero:
    mov rdi, 0
    div rdi
    ret

invalidOpcode:
    mov cr6, rax
    ret