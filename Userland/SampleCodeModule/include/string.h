#ifndef __STRING_H_
#define __STRING_H_
#include <stdint.h>
#include "stdio.h"
#define LOWERCASEDIFF 32

/**
 * Returns length of the received string
 */
int strlen(const char *str);

/**
 * Receives two strings and compares them.
 * Returns a negative integer if str1 is before str2
 * Returns positive integer if str1 is after str2
 * Returns 0 if both strings are the same
 *
 * | | | Case sensitive function | | |
 */
int strcmp(const char *str1, const char *str2);

/**
 * Receives two strings and compares them
 * Similar to strcmp but ignores uppercase
 * chars.
 */
int strcasecmp(const char *str1, const char *str2);

int strncasecmp(const char *str1, const char *str2, size_t n);

char * strtok (char *str, const char *delim);

int atoi(const char *str);



/**
 * Receives two strings as parameters
 * Copies the string in src to dest.
 * Returns a pointer to dest.
 */
char *strcpy(char *dest, const char *src);

/**
 * Receives a 64bits value
 * Returns a string of the value in hexadecimal
 * format with up to 8 digits.
 */
char *hexToString(uint64_t value);

/**
 * Receieves a string.
 * Returns the position of the first blank space and places a '\0' in that position.
 */
int divideString(char *str);

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

/**
 * Receives two strings as parameters.
 * Appends the string in src to the end of dest.
 * Returns a pointer to dest.
 */
char *strcat(char *dest, const char *src);

#endif