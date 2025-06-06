// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/stdio.h"
#include "include/syscalls.h"
#include "include/colors.h"
#include "include/string.h"
#include "include/lib.h"
#include <stdarg.h>
#define MAXBUFLEN 100
#define MINLEN 1
#define MAX_INPUTS_STORE 10

/**
 * readSizeFlag is used as a way of identifying whether
 * getChar returned with no value read because of timeout
 * or not
 */
static int readSizeFlag = 0;

static int ctrlFlag = 0, checkZoomFlag;

static uint64_t hexcol = DEFAULT; // default shell color

static int isPrintable(unsigned char c);
static int isVerticalArrow(unsigned char c);

static char inputs[MAX_INPUTS_STORE][MAXBUFLEN];
static int inputIndex = 0;

unsigned char getchar(void)
{
    unsigned char read = 0;
    int fds[2];
    sysGetFD(fds);
    readSizeFlag = sysReadScreen(fds[0], &read, 1);
    return read;
}

unsigned char putchar(unsigned char c)
{
    int fds[2];
    sysGetFD(fds);
    sysWriteScreen(fds[1], &c, 1, hexcol);
    return c;
}

int printColor(const char *str, uint64_t hexColor)
{
    hexcol = hexColor;
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        putchar(str[i]);
    }
    hexcol = DEFAULT;
    return i;
}

int printInt(int value)
{
    char buffer[MAXBUFLEN];
    intToString(value, buffer, MINLEN);
    return printColor(buffer, DEFAULT);
}

int print(char *str)
{
    return printColor(str, DEFAULT);
}

void intToHexString(int value, char *buffer, int minLen)
{
    const char *hexDigits = "0123456789ABCDEF";
    char temp[MAXBUFLEN];
    int index = 0;

    if (value == 0)
    {
        temp[index++] = '0';
    }
    else
    {
        while (value > 0)
        {
            temp[index++] = hexDigits[value % 16];
            value /= 16;
        }
    }

    // Rellenar con ceros si es necesario
    while (index < minLen)
    {
        temp[index++] = '0';
    }

    // Invertir el resultado
    for (int i = 0; i < index; i++)
    {
        buffer[i] = temp[index - i - 1];
    }
    buffer[index] = '\0';
}

int printf(char *str, ...)
{
    char buffer[MAXBUFLEN], numStr[MAXBUFLEN];
    int bufferIndex = 0;
    va_list args;
    va_start(args, str);

    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == '%' && str[i + 1] == 'd') // Formato decimal
        {
            i++;
            intToString(va_arg(args, int), numStr, MINLEN);
            for (int j = 0; numStr[j] != '\0'; ++j)
            {
                buffer[bufferIndex++] = numStr[j];
            }
        }
        else if (str[i] == '%' && str[i + 1] == 'x') // Formato hexadecimal
        {
            i++;
            intToHexString(va_arg(args, int), numStr, MINLEN);
            for (int j = 0; numStr[j] != '\0'; ++j)
            {
                buffer[bufferIndex++] = numStr[j];
            }
        }
        else
        {
            buffer[bufferIndex++] = str[i];
        }
    }
    buffer[bufferIndex] = '\0';
    va_end(args);
    return print(buffer);
}

int itoa(int value, char* buffer){

    int index = 0;
    if (value < 0) {
        buffer[index++] = '-';
        value = -value;
    }
    if (value == 0) {
        buffer[index++] = '0';
    } else {
        int temp = value;
        while (temp > 0) {
            temp /= 10;
            index++;
        }
        buffer[index] = '\0';
        while (value > 0) {
            buffer[--index] = (value % 10) + '0';
            value /= 10;
        }
    }
    return index; // Retorna la longitud del número convertido

}

int scanf(char *buffer, int size)
{
    if (size == 0)
        return 0;
    unsigned char read = 0;
    int readSize = 0, printedSize = 0;
    while (read != '\n')
    {
        ctrlFlag = sysCtrlPressed();
        read = getchar();
        if (!readSizeFlag)
            continue;

        if (ctrlFlag)
        {
            if (read == 'i')
            {
                checkZoomFlag = incTextSize();
                if (checkZoomFlag)
                {
                    print("   Maximum Size Reached.");
                    sysHideCursor();
                    sysPrintCursor();
                    sysSleep(0, 4);
                    for (int i = 0; i < strlen("   Maximum Size Reached."); i++)
                    {
                        putchar('\b');
                    }
                    sysShowCursor();
                    sysPrintCursor();
                }
            }
            else if (read == 'o')
            {
                checkZoomFlag = decTextSize();
                if (checkZoomFlag)
                {
                    print("   Minimum Size Reached.");
                    sysHideCursor();
                    sysPrintCursor();
                    sysSleep(0, 4);
                    for (int i = 0; i < strlen("   Minimum Size Reached."); i++)
                    {
                        putchar('\b');
                    }
                    sysShowCursor();
                    sysPrintCursor();
                }
            }
            else if (read == 'c'){
                printColor("Clearing screen...", YELLOW);
            }
            else if (read == 'd'){ 
                

            }

        }
        else
        {
            if (read == '\n')
            {
                buffer[readSize] = 0;
                if (readSize > 0)
                    strcpy(inputs[inputIndex++ % MAX_INPUTS_STORE], buffer);
                putchar(read); // Newline
            }
            else if (read == '\b' && readSize != 0)
            {
                if (readSize >= printedSize)
                    readSize--;
                buffer[readSize] = 0;
                putchar(read); // Backspace
                printedSize--;
            }
            else if (isVerticalArrow(read))
            {
                if (read == sysUpArrowValue())
                {
                    if (inputIndex > 0)
                    {
                        inputIndex--;
                    }
                }
                else
                {
                    if (inputIndex < MAX_INPUTS_STORE)
                    {
                        inputIndex++;
                    }
                }
                strcpy(buffer, inputs[inputIndex % MAX_INPUTS_STORE]);
                int commandLen = strlen(buffer);
                for (int i = 0; i < printedSize; i++)
                    putchar('\b');
                printedSize = commandLen;
                readSize = commandLen;
                print(buffer);
            }
            else if (isPrintable(read))
            { // Printable Character
                if (readSize != size)
                    buffer[readSize++] = read;
                putchar(read);
                printedSize++;
            }
        }
    }
    return readSize;
}

static int isPrintable(unsigned char c)
{
    return (c >= ' ' && c <= '~');
}

static int isVerticalArrow(unsigned char c)
{
    return c == sysUpArrowValue() || c == sysDownArrowValue();
}