GLOBAL sysWriteScreen
GLOBAL sysReadScreen
GLOBAL sysClearScreen
GLOBAL sysClearKbEntry

GLOBAL sysUpArrowValue
GLOBAL sysLeftArrowValue
GLOBAL sysDownArrowValue
GLOBAL sysRightArrowValue

GLOBAL sysPrintRectangle
GLOBAL sysPrintSquare

GLOBAL sysPrintRegs
GLOBAL sysSleep

GLOBAL sysGetSecs
GLOBAL sysGetMins
GLOBAL sysGetHour
GLOBAL sysGetDay
GLOBAL sysGetMonth
GLOBAL sysGetYear

GLOBAL sysBeepSound
GLOBAL sysZoomIn
GLOBAL sysZoomOut
GLOBAL sysGetZoomLevel
GLOBAL sysSetZoomLevel
GLOBAL sysPrintCursor
GLOBAL sysHideCursor
GLOBAL sysShowCursor
GLOBAL sysSetCursorPosition


GLOBAL sysCtrlPressed

GLOBAL sysCreateProcess
GLOBAL sysGetPID
GLOBAL sysGetParentPID
GLOBAL sysGetPS
GLOBAL sysFreePS
GLOBAL sysWait
GLOBAL sysKill
GLOBAL sysNice
GLOBAL sysBlock
global sysGetMemStatus

section .text

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

sysCreateProcess:
    mov rax,30
    int 80h
    ret

sysGetPID:
    mov rax, 31
    int 80h
    ret

sysGetParentPID:
    mov rax, 32
    int 80h
    ret

sysGetPS:
    mov rax, 33
    int 80h
    ret

sysFreePS:
    mov rax,34
    int 80h
    ret

sysWait:
    mov rax, 35
    int 80h
    ret

sysKill:
    mov rax,36
    int 80h
    ret

sysNice:
    mov rax, 37
    int 80h
    ret

sysBlock: 
    mov rax,38
    int 80h
    ret

sysGetMemStatus
    mov rax,39
    int 80h
    ret