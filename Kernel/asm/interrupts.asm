
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL _syscallHandler

GLOBAL getRegs
EXTERN getKey
EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscallDispatcher
EXTERN getStackBase

GLOBAL saveRegsInBuffer


SECTION .text




%macro saveRegsInBuffer 0	;; Once you enter here, regs[0]=RIP, regs[1]=RFLAGS, regs[2]=RSP
    mov [regs + 8*3], rax
    mov [regs + 8*4], rbp
    mov [regs + 8*5], rcx
    mov [regs + 8*6], rdx
    mov [regs + 8*7], rsi
    mov [regs + 8*8], rdi
    mov [regs + 8*9], rbx
    mov [regs + 8*10], r8
    mov [regs + 8*11], r9
    mov [regs + 8*12], r10
    mov [regs + 8*13], r11
    mov [regs + 8*14], r12
    mov [regs + 8*15], r13
    mov [regs + 8*16], r14
    mov [regs + 8*17], r15
%endmacro

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

%macro saveIntRegs 0

push rax
mov rax, [rsp + 8]	; RIP Contexto anterior
mov [regs], rax

mov rax, [rsp + 8*3] ; RFLAGS Contexto anterior
mov [regs + 8*1], rax

mov rax, [rsp + 8*4] ; RSP Contexto anterior
mov [regs + 8*2], rax

pop rax

%endmacro
   
%macro exceptionHandler 1
	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher
	
	call getStackBase

	mov [rsp+8*3], rax	;; Piso el RFLAGS

	mov rax, userland
	mov [rsp],rax		;; Piso la direccion de retorno 

	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

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


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	push rax
	call getKey
	cmp al, 0x38
	pop rax
	jne .continue
		saveIntRegs
		saveRegsInBuffer
		
.continue:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
_exception0Handler:
	saveIntRegs
	saveRegsInBuffer
	exceptionHandler 0

;Invalid OpCode Exception
_exception6Handler:
	saveIntRegs
	saveRegsInBuffer
	exceptionHandler 6


getRegs:
	mov rax,regs
	ret
	
;Syscall Handling
; _syscallHandler receives parameters in the next order: rax rdi rsi rdx r10 r8 r9
; syscallDispatcher receives parameters via regs this way: rdi rsi rdx rcx r8 r9
; rax is the last parameters -> r9 = rax
; r10 is not a parameters -> rcx = r10
_syscallHandler:
	;saveIntRegs
	mov rcx, r10
	mov r9, rax
	call syscallDispatcher
	iretq

haltcpu:
	cli
	hlt
	ret

section .data
regs dq 18
userland equ 0x400000