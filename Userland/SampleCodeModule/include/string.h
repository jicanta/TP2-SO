#ifndef __STRING_H_
#define __STRING_H_

#define LOWERCASEDIFF 32

/**
 * Returns length of the received string
*/
int strlen(const char * str);

/**
 * Receives two strings and compares them.
 * Returns a negative integer if str1 is before str2
 * Returns positive integer if str1 is after str2
 * Returns 0 if both strings are the same
 * 
 * | | | Case sensitive function | | |  
*/
int strcmp(const char * str1, const char * str2);

/**
 * Receives two strings and compares them
 * Similar to strcmp but ignores uppercase
 * chars.
*/
int strcasecmp(const char * str1, const char * str2);

/**
 * Receives two strings as parameters
 * Copies the string in src to dest.
 * Returns a pointer to dest.
*/
char * strcpy(char *dest, const char *src);

#endif