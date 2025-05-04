#ifndef IOLIB_H_
#define IOLIB_H_

#include <stdint.h>

#define STDIN 0
#define STDOUT 1

void putcharColor(char c, uint32_t color);

void putchar(char c);

char getchar();

int printf(const char *fmt, ...);

void println();

void clearScreen();

char * itoa(int num, char * str);
void printDims();

char * fgets(char *buffer, uint64_t size);

int scanf(const char *fmt, ...);

void puts(const char *str);

char toLower(char c);

#endif