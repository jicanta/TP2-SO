#ifndef _PIPES_H
#define _PIPES_H
#define MAX_FDS 1024

#include "semManager.h"
#include "lib.h"

#define BUFFER_SIZE 2048
#define STDIN 0
#define STDOUT 1
#define STDERR 2
typedef enum
{
    R = 1 << 0,
    W = 1 << 1,
    RW = R | W
} Mode;

typedef struct
{
    unsigned char buffer[BUFFER_SIZE];
    int readPos;
    int writePos;
    int referenceCountByMode[2];
    int eof;
    int dataAvailable;
    sem_t readSem;
    sem_t writeSem;
} Stream;

typedef struct
{
    Stream *resource;
    int isOpen;
    Mode mode;
} FD;

void initFileDescriptors();
int writeToFD(int fd, char *buf, uint64_t count, uint64_t hexColor);
int readFromFD(int fd, char *buf, uint64_t count);
int createPipe(int fds[2]);
int setEOF(int fd);
int closeFD(int fd);
int getReadPos(int fd);
int readFromFDAt(int fd, char *buf, uint64_t count, uint64_t pos);
#endif