#ifndef FILE_DESCRIPTORS_H
#define FILE_DESCRIPTORS_H

#define MAX_FDS 1024

#define BUFFER_SIZE 1024

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef enum {
    R = 0,     // Solo lectura
    W = 1,     // Solo escritura  
    RW = 2     // Lectura y escritura
} fd_mode_t;

typedef struct
{
    unsigned char buffer[BUFFER_SIZE];
    int readPos;
    int writePos;
    int dataAvailable;
    int eof;
    int readers;
    int writers;

} Stream;

typedef struct {

    int isOpen;
    Stream * resource;
    fd_mode_t mode;

} FD;

int initFileDescriptors();

int writeToFD(int fd, const char *buf, int count, unsigned long hexColor);
int readFromFD(int fd, char *buf, int count);
int createPipe(int fds[2]);
int getAvailableFD();

#endif