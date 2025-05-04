GLOBAL _sys_write
GLOBAL _sys_read
GLOBAL _sys_draw_rectangle
GLOBAL _sys_clear_screen
GLOBAL _sys_reset_position
GLOBAL _sys_jump_line
GLOBAL _sys_get_screen_info
GLOBAL _sys_get_registers
GLOBAL _sys_sleep
GLOBAL _sys_get_time
GLOBAL _sys_draw_string
GLOBAL _sys_inc_scale
GLOBAL _sys_dec_scale
GLOBAL _sys_play_sound
GLOBAL _sys_get_scale
GLOBAL _sys_set_scale
GLOBAL invalid_opcode


section .text

%macro syscall 1
    mov rax, %1
    int 80h
    ret
%endmacro

_sys_read: syscall 0x00
_sys_write: syscall 0x01
_sys_draw_rectangle: syscall 0x02
_sys_clear_screen: syscall 0x03
_sys_reset_position: syscall 0x04
_sys_jump_line: syscall 0x05
_sys_get_screen_info: syscall 0x06
_sys_get_registers: syscall 0x07
_sys_get_time : syscall 0x08
_sys_draw_string: syscall 0x09
_sys_sleep: syscall 0x0A
_sys_inc_scale : syscall 0x0B
_sys_dec_scale : syscall 0x0C 
_sys_play_sound : syscall 0x0D
_sys_get_scale : syscall 0x0E 
_sys_set_scale : syscall 0x0F

invalid_opcode: 

    mov cr6,rax
    ret 
