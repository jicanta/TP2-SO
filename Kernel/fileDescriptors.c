// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/fileDescriptors.h"
#include "../include/memoryManager.h"
#include "../include/videoDriver.h"
#include <process.h>

FD fileDescriptors[MAX_FDS];

void initFileDescriptors()
{
    for (int i = STDERR + 1; i < MAX_FDS; i++)
    {
        fileDescriptors[i].isOpen = 0;
    }

    fileDescriptors[0].isOpen = 1;
    fileDescriptors[0].mode = RW;
    fileDescriptors[0].resource = allocMemory(sizeof(Stream));
    fileDescriptors[0].resource->referenceCountByMode[0] = 1;
    fileDescriptors[0].resource->referenceCountByMode[1] = 1;
    fileDescriptors[0].resource->readPos = 0;
    fileDescriptors[0].resource->writePos = 0;
    fileDescriptors[0].resource->readSem = semCreate("/stdin_read", 0);
    fileDescriptors[0].resource->writeSem = semCreate("/stdin_write", BUFFER_SIZE);

    fileDescriptors[1].isOpen = 1;
    fileDescriptors[1].mode = W;
    fileDescriptors[1].resource = NULL;

    fileDescriptors[2].isOpen = 1;
    fileDescriptors[2].mode = W;
    fileDescriptors[2].resource = NULL;
}

int writeToFD(int fd, char *buf, uint64_t count, uint64_t hexColor)
{
    int written = 0;
    if (fd < 0 || fd >= MAX_FDS || !fileDescriptors[fd].isOpen || fileDescriptors[fd].mode == R)
        return -1;

    if (fd == STDOUT || fd == STDERR)
    {

        int color = (fd == STDERR) ? 0x00FF0000 : hexColor;
        char toPrint[2] = {0, 0};
        for (written = 0; written < count; written++)
        {
            toPrint[0] = buf[written];
            vdPrint(toPrint, color);
        }
        return written;
    }

    Stream *stream = fileDescriptors[fd].resource;
    if (stream->eof)
    {
        stream->eof = 0;
    }

    for (written = 0; written < count; written++)
    {
        semWait(stream->writeSem);
        stream->buffer[stream->writePos] = buf[written];
        stream->writePos = (stream->writePos + 1) % BUFFER_SIZE;
        stream->dataAvailable++;
        semPost(stream->readSem);
    }
    return written;
}

int readFromFD(int fd, char *buf, uint64_t count)
{
    int sizeRead = 0;
    if (fd < 0 || fd >= MAX_FDS || !fileDescriptors[fd].isOpen || fileDescriptors[fd].mode == W)
        return -1;

    unsigned char lastRead = '\0';
    Stream *stream = fileDescriptors[fd].resource;
    while (sizeRead != count)
    {

        if (stream->eof)
            semPost(stream->readSem);

        semWait(stream->readSem);
        if (!stream->eof || (fd != STDIN && (stream->referenceCountByMode[1] != 0 || stream->dataAvailable > 0)))
        {
            lastRead = stream->buffer[stream->readPos];
            stream->readPos = (stream->readPos + 1) % BUFFER_SIZE;
            stream->dataAvailable--;
            buf[sizeRead++] = lastRead;
            semPost(stream->writeSem);
        }
        else
        {
            stream->eof = 1;
            break;
        }
    }
    if (fd == STDIN)
    {
        stream->eof = 0;
    }
    return sizeRead;
}

int getAvailableFD()
{
    for (int i = 3; i < MAX_FDS; i++)
    {
        if (!fileDescriptors[i].isOpen)
        {
            fileDescriptors[i].isOpen = 1;
            return i;
        }
    }
    return -1;
}

int createPipe(int fds[2])
{
    int fd1 = getAvailableFD(), fd2 = getAvailableFD();
    if (fd1 == -1 || fd2 == -1)
        return -1;
    Stream *stream = allocMemory(sizeof(Stream));
    stream->referenceCountByMode[0] = 0;
    stream->referenceCountByMode[1] = 0;
    fileDescriptors[fd1].mode = R;
    fileDescriptors[fd1].resource = stream;
    fileDescriptors[fd1].resource->referenceCountByMode[0] += 1;
    fileDescriptors[fd1].resource->readPos = 0;
    char semName1[MAX_NAME_LENGTH];
    itoa(fd1, semName1);
    fileDescriptors[fd1].resource->readSem = semCreate(semName1, 0);

    fileDescriptors[fd2].mode = W;
    fileDescriptors[fd2].resource = stream;
    fileDescriptors[fd2].resource->writePos = 0;
    char semName2[MAX_NAME_LENGTH];
    itoa(fd2, semName2);
    fileDescriptors[fd2].resource->writeSem = semCreate(semName2, BUFFER_SIZE);
    fileDescriptors[fd2].resource->referenceCountByMode[1] += 1;
    fds[0] = fd1;
    fds[1] = fd2;
    return 0;
}

int closeFD(int fd)
{
    if (fd < 0 || fd == STDOUT || fd == STDERR || fd >= MAX_FDS || !fileDescriptors[fd].isOpen)
        return -1;
    if (fd == STDIN)
    {
        fileDescriptors[fd].resource->eof = 0;
        return 0;
    }

    if (fileDescriptors[fd].mode & R)
        fileDescriptors[fd].resource->referenceCountByMode[0] -= 1;

    if (fileDescriptors[fd].mode & W)
        fileDescriptors[fd].resource->referenceCountByMode[1] -= 1;

    if (fileDescriptors[fd].resource->referenceCountByMode[0] > 0 && fileDescriptors[fd].resource->referenceCountByMode[1] <= 0)
    {
        fileDescriptors[fd].resource->eof = 1;
        for (int i = 0; i < fileDescriptors[fd].resource->referenceCountByMode[0]; i++)
        {
            semPost(fileDescriptors[fd].resource->readSem);
        }
    }

    if (fileDescriptors[fd].resource->referenceCountByMode[0] == 0 && fileDescriptors[fd].resource->referenceCountByMode[1] == 0)
    {
        fileDescriptors[fd].resource->eof = 0;
        semClose(fileDescriptors[fd].resource->readSem);
        semClose(fileDescriptors[fd].resource->writeSem);
        freeMemory(fileDescriptors[fd].resource);
    }

    fileDescriptors[fd].isOpen = 0;
    return 0;
}

int setEOF(int fd)
{
    if (fd < 0 || fd >= MAX_FDS || fd == STDOUT || fd == STDERR || !fileDescriptors[fd].isOpen || fileDescriptors[fd].mode == R)
        return -1;
    Stream *stream = fileDescriptors[fd].resource;
    stream->eof = 1;
    semPost(stream->readSem);
    return 0;
}

int getReadPos(int fd)
{
    if (fd < 0 || fd >= MAX_FDS || fd == STDOUT || fd == STDERR || !fileDescriptors[fd].isOpen || fileDescriptors[fd].mode == W)
        return -1;
    return fileDescriptors[fd].resource->readPos;
}

int readFromFDAt(int fd, char *buf, uint64_t count, uint64_t pos)
{
    int sizeRead = 0;
    if (fd < 0 || fd >= MAX_FDS || !fileDescriptors[fd].isOpen || fileDescriptors[fd].mode == W)
        return -1;

    unsigned char lastRead = '\0';
    Stream *stream = fileDescriptors[fd].resource;
    if (pos >= BUFFER_SIZE)
        return -1;

    if (stream->dataAvailable > 0)
    {
        if (stream->buffer[pos] != '\0')
        {
            semWait(stream->readSem);
            lastRead = stream->buffer[pos];
            buf[sizeRead++] = lastRead;
        }
    }
    if (pos == stream->readPos && stream->dataAvailable > 0)
    {
        stream->readPos = (stream->readPos + 1) % BUFFER_SIZE;
        (stream->dataAvailable)--;
    }
    return sizeRead;
}