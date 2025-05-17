GLOBAL invalid_opcode
GLOBAL dispatcherWrite
GLOBAL dispatcherRead
GLOBAL dispatcherDrawRectangle
GLOBAL dispatcherClearScreen
GLOBAL dispatcherResetPosition
GLOBAL dispatcherJumpLine
GLOBAL dispatcherGetScreenInfo
GLOBAL dispatcherGetRegisters
GLOBAL dispatcherSleep
GLOBAL dispatcherGetTime
GLOBAL dispatcherDrawString
GLOBAL dispatcherIncScale
GLOBAL dispatcherDecScale
GLOBAL dispatcherGetScale
GLOBAL dispatcherSetScale
GLOBAL dispatcherInitMemory
GLOBAL dispatcherMalloc
GLOBAL dispatcherFree
GLOBAL dispatcherMemStatus


GLOBAL sysMalloc
GLOBAL sysFree
GLOBAL sysGetMemState

section .text

<<<<<<< HEAD
sysReadScreen:         ; RDI: fileDescriptor, RSI: buffer, RDX: sizeToRead
    mov rax, 0
    int 80h
    ret

sysWriteScreen:         ; RDI: fileDescriptor, RSI: buffer, RDX: sizeToPrint, R10: colorHex
    mov r10, rcx
    mov rax, 1
    int 80h
    ret

sysPrintRegs:
    mov rax, 2
    int 80h
    ret

sysZoomIn:
    mov rax, 3
    int 80h
    ret

sysZoomOut:
    mov rax, 4
    int 80h
    ret

sysGetZoomLevel:
    mov rax, 5
    int 80h
    ret

sysSetZoomLevel:
    mov rax, 6
    int 80h
    ret

sysUpArrowValue:
    mov rax, 7
    int 80h
    ret

sysLeftArrowValue:
    mov rax, 8
    int 80h
    ret

sysDownArrowValue:
    mov rax, 9
    int 80h
    ret

sysRightArrowValue:
    mov rax, 10
    int 80h
    ret

sysClearScreen:
    mov rax, 11
    int 80h
    ret

sysPrintSquare:
    mov r10, rcx
    mov rax, 12
    int 80h
    ret

sysPrintRectangle:
    mov r10, rcx
    mov rax, 13
    int 80h
    ret

sysSetCursorPosition:
    mov rax, 14
    int 80h
    ret

sysBeepSound:
    mov rax, 15
    int 80h
    ret

sysSleep:
    mov rax, 16
    int 80h
    ret

sysHideCursor:
    mov rax, 17
    int 80h
    ret

sysShowCursor:
    mov rax, 18
    int 80h
    ret

sysPrintCursor:
    mov rax, 19
    int 80h
    ret

sysGetSecs:
    mov rax, 20
    int 80h
    ret

sysGetMins:
    mov rax, 21
    int 80h
    ret

sysGetHour:
    mov rax, 22
    int 80h
    ret

sysGetDay:
    mov rax, 23
    int 80h
    ret

sysGetMonth:
    mov rax, 24
    int 80h
    ret

sysGetYear:
    mov rax, 25
    int 80h
    ret

sysCtrlPressed:
    mov rax, 26
    int 80h
    ret

sysClearKbEntry:
    mov rax, 27
    int 80h
    ret

sysMalloc:
    mov rax, 28
    int 80h
    ret

sysFree:
    mov rax, 29
    int 80h
    ret

sysGetMemState:
    mov rax, 30
    int 80h
    ret
=======
%macro syscall 1
    mov rax, %1
    int 80h
    ret
%endmacro

dispatcherRead: syscall 0x00
dispatcherWrite: syscall 0x01
dispatcherDrawRectangle: syscall 0x02
dispatcherClearScreen: syscall 0x03
dispatcherResetPosition: syscall 0x04
dispatcherJumpLine: syscall 0x05
dispatcherGetScreenInfo: syscall 0x06
dispatcherGetRegisters: syscall 0x07
dispatcherGetTime : syscall 0x08
dispatcherDrawString: syscall 0x09
dispatcherSleep: syscall 0x0A
dispatcherIncScale : syscall 0x0B
dispatcherDecScale : syscall 0x0C 
dispatcherGetScale : syscall 0x0E 
dispatcherSetScale : syscall 0x0F
; snapshot : syscall 0x10
dispatcherFree : syscall 0x11
dispatcherMemStatus : syscall 0x12
dispatcherMalloc : syscall 0x13
invalid_opcode: 

    mov cr6,rax
    ret 
>>>>>>> parent of 7a25ff6 ([IMPORTANT] change kernel)
