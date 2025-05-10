#ifndef TPE_ARQ_SHELL_H
#define TPE_ARQ_SHELL_H

#include <iolib.h>
#include <color.h>
#include <commands.h>
#include <stringutils.h>
#include <syscallsDispatchers.h>

#define UP_ARROW 0x41
#define BACKSPACE 0x08
#define ESC 27
#define BUFFER_SIZE 256

typedef struct buffer {
    char items[BUFFER_SIZE];
    int size;
} TBuffer;

/**
 * @brief Starts the shell.
 */
void shell();

#endif //TPE_ARQ_SHELL_H