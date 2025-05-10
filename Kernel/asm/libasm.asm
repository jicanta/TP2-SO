GLOBAL cpuVendor
GLOBAL get_key
GLOBAL all_registers
GLOBAL rtc
GLOBAL dispatcherGetRegisters
GLOBAL _outb
GLOBAL _inb

EXTERN set_registers

section .text

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid

	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

; get_key solo se debe ejecutar si el primer bit de 64h está en 1
get_key:
	push rbp
	mov rbp, rsp

	in al, 60h

	leave
	ret

rtc:
    push dx
    mov dx, 0x70
    mov al, dil
    out dx, al
    xor rax, rax
    in al, 0x71
    pop dx
    ret

; void outb(uint16_t port, uint8_t value)
_outb:
    push rdx

    mov dx, di
    mov al, sil
    out dx, al

    pop rdx
    ret

; extern uint8_t inb(uint16_t port)
_inb:
    push rdx

    mov dx, di
    in al, dx

    pop rdx
    ret