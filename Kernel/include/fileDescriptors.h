#ifndef FILE_DESCRIPTORS_H
#define FILE_DESCRIPTORS_H

#define MAX_FDS 1024

#define BUFFER_SIZE 2048

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef enum {
    R = 0,
    W = 1,  
    RW = 2
} fdMode;

typedef struct
{
    unsigned char buffer[BUFFER_SIZE];
    int readPos;
    int writePos;
    int dataAvailable;
    int eof;
    int readers;
    int writers;
    int readSem;
    int writeSem;

} Stream;

typedef struct {

    int isOpen;
    Stream * resource;
    fdMode mode;

} FD;

/*
 * Initializes the file descriptor system.
 * Returns 0 on success, or a negative value on failure.
 */
int initFileDescriptors();

/*
 * Writes data to the file descriptor specified by fd.
 * Parameters:
 *   fd - The file descriptor to write to.
 *   buf - The buffer containing the data to write.
 *   count - The number of bytes to write.
 *   hexColor - The color to use for the output (if applicable).
 * Returns the number of bytes written, or a negative value on error.
 */
int writeToFD(int fd, const char *buf, int count, unsigned long hexColor);

/*
 * Reads data from the file descriptor specified by fd.
 * Parameters:
 *   fd - The file descriptor to read from.
 *   buf - The buffer to store the read data.
 *   count - The maximum number of bytes to read.
 * Returns the number of bytes read, or a negative value on error.
 */
int readFromFD(int fd, char *buf, int count);

/*
 * Creates a pipe and assigns two file descriptors to it.
 * Parameters:
 *   fds - An array to store the two file descriptors for the pipe.
 * Returns 0 on success, or a negative value on failure.
 */
int createPipe(int fds[2]);

/*
 * Retrieves an available file descriptor.
 * Returns the file descriptor number, or a negative value if none are available.
 */
int getAvailableFD();

/*
 * Closes the file descriptor specified by fd.
 * Parameters:
 *   fd - The file descriptor to close.
 * Returns 0 on success, or a negative value on failure.
 */
int closeFD(int fd);

/*
 * Sets the EOF (End of File) flag for the file descriptor specified by fd.
 * Parameters:
 *   fd - The file descriptor to set EOF for.
 * Returns 0 on success, or a negative value on failure.
 */
int setEOF(int fd);

/*
 * Reads data from the file descriptor specified by fd at a specific position.
 * Parameters:
 *   fd - The file descriptor to read from.
 *   buf - The buffer to store the read data.
 *   count - The maximum number of bytes to read.
 *   pos - The position to start reading from.
 * Returns the number of bytes read, or a negative value on error.
 */
int readFromFDAt(int fd, char *buf, int count, int pos);

/*
 * Retrieves the current read position for the file descriptor specified by fd.
 * Parameters:
 *   fd - The file descriptor to query.
 * Returns the read position, or a negative value on error.
 */
int getReadPos(int fd);


#endif