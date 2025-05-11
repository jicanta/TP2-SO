GLOBAL getKey

section .text
getKey:
    cli
    xor rax, rax
    in al, 60h      ; donde deja la tecla que se leyo (lo deja en formato scancode)
    sti
    ret