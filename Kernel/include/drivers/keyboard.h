#ifndef TPE_ARQ_KEYBOARDDRIVER_H
#define TPE_ARQ_KEYBOARDDRIVER_H

#include <stdint.h>

#define UP_ARROW 0x41
#define ESC 27
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define LCTRL 0x1D
#define LCTRL_RELEASE 0x9D
#define CAPSLOCK 0x3A
#define RELEASE_OFFSET 0x80
#define KEYS_AMOUNT 59
#define ESC 27
#define BUFFER_SIZE 256

/**
 * @brief Manejador de interrupciones de hardware para el teclado.
 *
 * @return 1 si se presionó la tecla 'ESC', 0 en caso contrario.
 */
uint64_t keyboard_handler();

/**
 * @brief Devuelve un carácter del búfer del teclado
 * o 0 si el búfer está vacío.
 */
char kb_getchar();

#endif
