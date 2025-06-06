#include "fileDescriptors.h"
#include "videoDriver.h"
#include "keyboard.h"
#include "syscallHandle.h"

FD fileDescriptors[MAX_FDS];

int initFileDescriptors() {
    // Inicializar todos los FDs como cerrados
    for (int i = 0; i < MAX_FDS; i++) {
        fileDescriptors[i].isOpen = 0;
        fileDescriptors[i].resource = 0;
    }
    
    // STDIN - Terminal para lectura
    fileDescriptors[STDIN].isOpen = 1;
    fileDescriptors[STDIN].mode = RW;
    fileDescriptors[STDIN].resource = myMalloc(sizeof(Stream));
    fileDescriptors[STDIN].resource->readPos = 0;
    fileDescriptors[STDIN].resource->writePos = 0;
    fileDescriptors[STDIN].resource->dataAvailable = 0;
    fileDescriptors[STDIN].resource->eof = 0;
    fileDescriptors[STDIN].resource->readers = 1;
    fileDescriptors[STDIN].resource->writers = 1;

    
    // STDOUT - Terminal para escritura
    fileDescriptors[STDOUT].isOpen = 1;
    fileDescriptors[STDOUT].mode = W;
    fileDescriptors[STDOUT].resource = 0;

    
    // STDERR - Terminal para escritura de errores
    fileDescriptors[STDERR].isOpen = 1;
    fileDescriptors[STDERR].mode = W;
    fileDescriptors[STDERR].resource = 0;
    
    return 0;
}

int readFromFD(int fd, char *buf, int count) {
    
    int sizeRead = 0;

    unsigned char lastRead = '\0';

    Stream * stream = fileDescriptors[fd].resource;

    while(sizeRead != count){

        if(stream->dataAvailable){
            
            lastRead = stream->buffer[stream->readPos];
            
            stream->readPos = (stream->readPos + 1) % BUFFER_SIZE;
            stream->dataAvailable--;
            buf[sizeRead++] = lastRead;
        } else {
            break;
        }

    }

    return sizeRead;
}

int writeToFD(int fd, const char *buf, int count, unsigned long hexColor) {

    int written = 0;

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

    Stream * stream = fileDescriptors[fd].resource;

   // vdPrint("\nWriting to stream...\n", 0xF0F0F0);

    for(written =0 ; written < count; written++){
        stream->buffer[stream->writePos] = buf[written];
        stream->writePos = (stream->writePos + 1) % BUFFER_SIZE;
        stream->dataAvailable++;
    }
    
    return written;
}


int getAvailableFD(){

    for(int i = 3; i<MAX_FDS;i++){
        if(!fileDescriptors[i].isOpen){
            fileDescriptors[i].isOpen = 1;
            return i;
        }
    }
    
    return -1;
}

int createPipe(int fds[2]){

    int fd1 = getAvailableFD(), fd2 = getAvailableFD();

    Stream * stream = myMalloc(sizeof(Stream));

    fileDescriptors[fd1].resource = stream;
    fileDescriptors[fd1].isOpen = 1;
    fileDescriptors[fd1].mode = R;
    fileDescriptors[fd1].resource->readPos = 0;
    fileDescriptors[fd1].resource->writePos = 0;
    fileDescriptors[fd1].resource->dataAvailable = 0;
    fileDescriptors[fd1].resource->eof = 0;

    fileDescriptors[fd2].resource = stream;
    fileDescriptors[fd2].isOpen = 1;
    fileDescriptors[fd2].mode = W;
    fileDescriptors[fd2].resource->readPos = 0;
    fileDescriptors[fd2].resource->writePos = 0;
    fileDescriptors[fd2].resource->dataAvailable = 0;
    fileDescriptors[fd2].resource->eof = 0;

    fds[0] = fd1;
    fds[1] = fd2;

    return 0;
}

int closeFD(int fd) {
    if (fd < 0 || fd >= MAX_FDS || !fileDescriptors[fd].isOpen) {
        return -1; // FD inválido o no abierto
    }

    if(fileDescriptors[fd].resource->writers == 0 && fileDescriptors[fd].resource->readers == 0){
        freeMemory(fileDescriptors[fd].resource);
    }
    fileDescriptors[fd].isOpen = 0;
    fileDescriptors[fd].resource = 0; // Liberar el recurso asociado
    
    return 0; // Éxito
}

