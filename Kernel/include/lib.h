#ifndef LIB_H
#define LIB_H

#include <stdint.h>

/*
 * Sets a block of memory to a specific character.
 * Parameters:
 *   destination - Pointer to the memory block to set.
 *   character - The value to set each byte to.
 *   length - The number of bytes to set.
 * Returns a pointer to the memory block.
 */
void * memset(void * destination, int32_t character, uint64_t length);

/*
 * Copies a block of memory from source to destination.
 * Parameters:
 *   destination - Pointer to the destination memory block.
 *   source - Pointer to the source memory block.
 *   length - The number of bytes to copy.
 * Returns a pointer to the destination memory block.
 */
void * memcpy(void * destination, const void * source, uint64_t length);

/*
 * Saves the current state of the CPU registers.
 */
void saveRegs(void);

/*
 * Prints the saved CPU registers.
 * Returns 0 on success, or a negative value on failure.
 */
int regPrinting(void);

/*
 * Calculates the length of a null-terminated string.
 * Parameters:
 *   str - The string to calculate the length of.
 * Returns the length of the string.
 */
int strlen(const char *str);

/*
 * Compares two null-terminated strings.
 * Parameters:
 *   str1 - The first string to compare.
 *   str2 - The second string to compare.
 * Returns 0 if the strings are equal, a negative value if str1 is less than str2, or a positive value if str1 is greater than str2.
 */
int strcmp(const char *str1, const char *str2);

/*
 * Converts an integer value to a null-terminated string.
 * Parameters:
 *   value - The integer value to convert.
 * Returns a pointer to the resulting string.
 */
char * itoa(int value);

#endif