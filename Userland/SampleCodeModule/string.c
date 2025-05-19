// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/string.h"
#include "include/stdio.h"

static void toLower(char *str);

int strlen(const char *str)
{
    int size;
    for (size = 0; str[size] != '\0'; size++)
        ;
    return size;
}

int strcmp(const char *str1, const char *str2)
{
    int i, j;
    i = j = 0;
    while (str1[i] != '\0' && (str1[i] == str2[j]))
    {
        i++;
        j++;
    }
    return str1[i] - str2[j];
}

char *strcpy(char *dest, const char *src)
{
    int len = strlen(src), i;
    for (i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

int strcasecmp(const char *str1, const char *str2)
{
    int len1 = strlen(str1), len2 = strlen(str2);
    char s1[len1], s2[len2];
    strcpy(s1, str1);
    strcpy(s2, str2);
    toLower(s1);
    toLower(s2);
    return strcmp(s1, s2);
}

/**
 * Turns str received as a parameter to lowerCase
 */
static void toLower(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] += LOWERCASEDIFF;
        }
    }
}

char *hexToString(uint64_t value)
{
    static char str[17];
    str[16] = '\0';

    char hex[] = "0123456789ABCDEF";

    uint64_t mask = 0xF;

    for (int i = 15; i >= 0; --i)
    {
        int digit = (value >> (i * 4)) & mask;
        str[15 - i] = hex[digit];
    }

    return str + 8;
}

int divideString(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == ' ')
        {
            str[i] = '\0';
            return i;
        }
    }
    return -1;
}