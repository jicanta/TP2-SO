GLOBAL rtc_get_seconds
GLOBAL rtc_get_minutes
GLOBAL rtc_get_hour
GLOBAL rtc_get_day
GLOBAL rtc_get_month
GLOBAL rtc_get_year

section .text

%macro rtc_get 1
    cli
    ; Argumento: registro a leer (en AL)
    ; Lee el registro del RTC y devuelve el valor leído en AL
    ; Selecciona el registro a leer

    xor rax, rax
    mov al, 0xB
    out 0x70, al 

    ; Lee el valor del registro
    in al, 0x71
    or al, 4
    out 0x71, al    ; Sets data to be returned in binary

    mov al, %1      ; Argumento (registro a leer)
    out 70h, al
    in al, 71h

    sti
%endmacro

rtc_get_seconds:
    rtc_get 0x00

    ret

rtc_get_minutes:
    rtc_get 0x02

    ret

rtc_get_hour:
    rtc_get 0x04

    ret

rtc_get_day:        ; Returns day of the month
    rtc_get 0x07

    ret

rtc_get_month:
    rtc_get 0x08

    ret

rtc_get_year:
    rtc_get 0x09

    ret