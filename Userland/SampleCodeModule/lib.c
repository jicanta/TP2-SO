// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/lib.h"
#include "include/syscalls.h"

int incTextSize()
{
    return sysZoomIn();
}

int decTextSize()
{
    return sysZoomOut();
}

int intLength(int num, int minLen)
{
    int length = 0;
    while (num != 0)
    {
        length++;
        num /= 10;
    }
    return length < minLen ? minLen : length;
}

void intToString(int num, char *buf, int minLen)
{
    int length = intLength(num, minLen);

    // Convert each digit to a char
    for (int i = length - 1; i >= 0; i--)
    {
        buf[i] = (num % 10) + '0';
        num /= 10;
    }
    buf[length] = '\0';
}

void stringToInt(char *str, int *num)
{
    *num = 0;
    while (*str != '\0')
    {
        *num = *num * 10 + (*str - '0');
        str++;
    }
}

int createProcess(creationParameters *params)
{
    return sysCreateProcess(params);
}