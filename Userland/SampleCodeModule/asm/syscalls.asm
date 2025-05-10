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


section .text

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
