GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL _sti
GLOBAL _cli
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception1Handler

GLOBAL regs_snapshot
GLOBAL exception_regs
GLOBAL snapshot
GLOBAL take_snapshot

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscall_handler
EXTERN getStackBase


SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

; Instruccion INT pushea RFLAGS, RSP y otros registros.
%macro take_snapshot_macro 0
    mov [regs_snapshot + 8*0], rax
    mov [regs_snapshot + 8 * 1 ], rbx
    mov [regs_snapshot + 8 * 2 ], rcx
    mov [regs_snapshot + 8 * 3 ], rdx
    mov [regs_snapshot + 8 * 4 ], rsi
    mov [regs_snapshot + 8 * 5 ], rdi
    mov [regs_snapshot + 8 * 6 ], rbp
    mov rax, [rsp + 18 * 8]						; RSP (Pusheado por INT)
    mov [regs_snapshot + 8 * 7 ], rax
    mov [regs_snapshot + 8 * 8 ], r8
    mov [regs_snapshot + 8 * 9 ], r9
    mov [regs_snapshot + 8 * 10], r10
    mov [regs_snapshot + 8 * 11], r11
    mov [regs_snapshot + 8 * 12], r12
    mov [regs_snapshot + 8 * 13], r13
    mov [regs_snapshot + 8 * 14], r14
    mov [regs_snapshot + 8 * 15], r15
    mov rax, [rsp + 15*8]    					; RIP (Es la dir de retorno)
    mov [regs_snapshot + 8 * 16], rax

    mov rax, 1
    mov [snapshot], rax
%endmacro


; INTERRUPCIONES HARDWARE
;--------------------------------------------------------
%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;keyboard
_irq01Handler:
	pushState

	mov rdi, 1 ; pasaje de parametro
	call irqDispatcher

	cmp DWORD [take_snapshot], 1	; Si la flag vale 1, tomamos snapshot
	jne .end

; Tomar snapshot
	popState
	pushState

	take_snapshot_macro

.end:
	mov al, 20h
	out 20h, al

	popState
	iretq
;--------------------------------------------------------


; MANEJO SYSCALLS
;--------------------------------------------------------
;Syscall
_irq80Handler:
	pushState

	cmp rax, 0x10	; si la syscall es take_snapshot salta a .take_snapshot
	je .take_snapshot

	mov rdi, rsp
	call syscall_handler
	jmp .end

.take_snapshot:
	take_snapshot_macro

.end:
	popState
	iretq
;--------------------------------------------------------



; EXCEPCIONES
;--------------------------------------------------------
%macro exceptionHandler 1
	cli
	pushState

	mov [exception_regs + 8*0 ], rax
	mov [exception_regs + 8*1 ], rbx
	mov [exception_regs + 8*2 ], rcx
	mov [exception_regs + 8*3 ], rdx
	mov [exception_regs + 8*4 ], rsi
	mov [exception_regs + 8*5 ], rdi
	mov [exception_regs + 8*6 ], rbp
	mov rax, [rsp + 18 * 8]									; RSP 
	mov [exception_regs + 8*7 ], rax
	mov [exception_regs + 8*8 ], r8
	mov [exception_regs + 8*9 ], r9
	mov [exception_regs + 8*10], r10
	mov [exception_regs + 8*11], r11
	mov [exception_regs + 8*12], r12
	mov [exception_regs + 8*13], r13
	mov [exception_regs + 8*14], r14
	mov [exception_regs + 8*15], r15
	mov rax, [rsp+15*8]                     ; RIP
	mov [exception_regs + 8*16], rax
	mov rax, [rsp+17*8]                     ; RFLAGS (pusheado por INT)
	mov [exception_regs + 8*17], rax

	mov rdi, %1 ; pasaje de parametro
	mov rsi, exception_regs
	call exceptionDispatcher

	popState
	
	; PARA PODER RETORNAR CORRECTAMENTE
	call getStackBase
	mov [rsp+24], rax
	mov rax, userland
	mov [rsp], rax

	sti
	iretq
%endmacro

;Zero Division
_exception0Handler:
	exceptionHandler 0

;Invalid Op Code
_exception1Handler:
	exceptionHandler 1
;--------------------------------------------------------



; OTRAS FUNCIONES
;--------------------------------------------------------
; picMasterMask
; Hay 8 perifericos. Se necesita 1 byte. Se recibe por dil.
; se almacena dil en 21h, que apunta a la mascara del pic master
;--------------------------------------------------------
picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

    
; sti y cli son instrucciones de assembler que habilitan y deshabilitan las interrupciones respectivamente
_sti:
	sti
	ret

_cli:
	cli
	ret

_hlt:
	hlt
	ret


; DECLARACION DATOS
;--------------------------------------------------------
section .data
	regs_snapshot resq 17
	snapshot dq 0				; flag para saber si hay una snapshot tomada
	exception_regs resq 18
	take_snapshot dq 0	; flag para saber si hay que tomar snapshot

section .rodata
    userland equ 0x400000