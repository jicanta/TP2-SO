
/**
 *  0   =   =   =   0   =   =   =   0   =   =   =   0   =   =   =   0   =   =   =   0
 *                              General purpose library
 *  0   =   =   =   0   =   =   =   0   =   =   =   0   =   =   =   0   =   =   =   0
 */

#ifndef __LIB_H_
#define __LIB_H_

#include "./processStructure.h"

/**
 * Increments text size on screen.
 * Returns 0 if increment was successful, 1 if not (In case max. size has been reached).
 */
int incTextSize(void);

/**
 * Decrements text size on screen.
 * Returns 0 if decrement was successful, 1 if not (In case min. size has been reached).
 *
 */
int decTextSize(void);

/**
 * Receives a num and a minLen.
 * Returns the length of the num
 * If the calculated length is lower than minLen, returns minLen.
 *
 */
int intLength(int num, int minLen);

/**
 * Receives a num, buffer and minLen. Copies the number in the buffer.
 * If the number of digits is lower than minLen, then completes it with
 * 0's up to minLen
 * */

void intToString(int num, char *buf, int minLen);

/**
 * Used to create a new process
 */
int createProcess(creationParameters *params);

/**
 * Receives a string and a pointer to an int.
 * Converts the string to an int and leaves it in num.
 */
void stringToInt(char *str, int *num);

#endif