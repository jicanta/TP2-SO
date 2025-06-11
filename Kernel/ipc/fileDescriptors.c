#include "fileDescriptors.h"
#include "videoDriver.h"
#include "keyboard.h"
#include "syscallHandle.h"
#include "lib.h"

FD fileDescriptors[MAX_FDS];


int initFileDescriptors() {
    // Inicializar todos los FDs como cerrados
    for (int i = 0; i < MAX_FDS; i++) {
        fileDescriptors[i].isOpen = 0;
        fileDescriptors[i].resource = NULL;
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
    fileDescriptors[STDIN].resource->readSem = semCreate("hola1",0); // Semáforo de lectura
    fileDescriptors[STDIN].resource->writeSem = semCreate("hola2", BUFFER_SIZE); // Semáforo de escritura

    
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

     if (fd == STDIN && getCurrentProcess()->foreground == 0) {
        vdPrint("Error: STDIN cannot be read in background processes.\n", 0xFF0000);
        return 0; // EOF - indica "no hay más datos"
    }

    int sizeRead = 0;

    unsigned char lastRead = '\0';

    Stream * stream = fileDescriptors[fd].resource;

    while(sizeRead != count){

        if(stream->eof){
            semPost(stream->readSem);
        }

        semWait(stream->readSem);

        if (!stream->eof || (fd != STDIN && (stream->writers != 0 || stream->dataAvailable > 0))){
            
            lastRead = stream->buffer[stream->readPos];
            
            stream->readPos = (stream->readPos + 1) % BUFFER_SIZE;
            stream->dataAvailable--;
           
            buf[sizeRead++] = lastRead;
           
            semPost(stream->writeSem);
        } else {
            stream->eof = 1; // Set EOF state if no more data is available
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

    if(stream->eof){
        stream->eof = 0; // Reset EOF state
    }


    for(written =0 ; written < count; written++){

        semWait(stream->writeSem);

        stream->buffer[stream->writePos] = buf[written];
        stream->writePos = (stream->writePos + 1) % BUFFER_SIZE;
        stream->dataAvailable++;
        
        semPost(stream->readSem);
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

    if(fd1 < 0 || fd2 < 0) {
        return -1; // No hay descriptores de archivo disponibles
    }

    Stream * stream = myMalloc(sizeof(Stream));

    fileDescriptors[fd1].resource = stream;
    fileDescriptors[fd1].isOpen = 1;
    fileDescriptors[fd1].mode = R;
    fileDescriptors[fd1].resource->readPos = 0;
    fileDescriptors[fd1].resource->writePos = 0;
    fileDescriptors[fd1].resource->dataAvailable = 0;
    fileDescriptors[fd1].resource->eof = 0;
    fileDescriptors[fd1].resource->readers = 1;
    fileDescriptors[fd1].resource->writers = 1;
    fileDescriptors[fd1].resource->readSem = semCreate(itoa(fd1), 0); // Semáforo de lectura
    fileDescriptors[fd1].resource->writeSem = semCreate(itoa(fd2), BUFFER_SIZE); // Semáforo de escritura

    fileDescriptors[fd2].resource = stream;
    fileDescriptors[fd2].isOpen = 1;
    fileDescriptors[fd2].mode = W;
    

    fds[0] = fd1;
    fds[1] = fd2;

    return 0;
}

int closeFD(int fd) {
 

    //vdPrint("Closing FD: ", 0x0000FF);
   if (fd < 0 || fd == STDOUT || fd == STDERR || fd >= MAX_FDS || !fileDescriptors[fd].isOpen)
        return -1;


    if (fd == STDIN)
    {
        fileDescriptors[fd].resource->eof = 0;
        return 0;
    }    

    if (fileDescriptors[fd].mode == R){

        fileDescriptors[fd].resource->readers -= 1;  
        
    }                                                                                                                                                                                                                                                                                                                                                                                   

    if (fileDescriptors[fd].mode == W)  {
        
        fileDescriptors[fd].resource->writers -= 1;  
        
    }                                                                                                                                                                                                                                                

  
    
    if (fileDescriptors[fd].resource->readers > 0 && fileDescriptors[fd].resource->writers <= 0)
    {

        fileDescriptors[fd].resource->eof = 1;
        for (int i = 0; i < fileDescriptors[fd].resource->readers; i++)
        {

            semPost(fileDescriptors[fd].resource->readSem);
        }
    }


    if (fileDescriptors[fd].resource->readers == 0 && fileDescriptors[fd].resource->writers == 0)
    {
        fileDescriptors[fd].resource->eof = 0;
        semDestroy(fileDescriptors[fd].resource->readSem);
        semDestroy(fileDescriptors[fd].resource->writeSem);
        freeMemory(fileDescriptors[fd].resource);
    }

    fileDescriptors[fd].isOpen = 0;
    return 0;


    
}

int setEOF(int fd){
    if (fd < 0 || fd >= MAX_FDS || fd == STDOUT || fd == STDERR || !fileDescriptors[fd].isOpen || fileDescriptors[fd].mode == R)
        return -1;
    Stream *stream = fileDescriptors[fd].resource;
    stream->eof = 1;
    semPost(stream->readSem);
    return 0;
}
