#include "include/stdio.h"
#include "include/syscalls.h"
#include "include/colors.h"
#include "include/string.h"
#include "include/lib.h"
#include <stdarg.h>

#define PRINTF_BUFFER_SIZE 100
#define MAXBUFLEN 100
#define MINLEN 2
#define MAX_INPUTS_STORE    10

/**
 * readSizeFlag is used as a way of identifying whether
 * getChar returned with no value read because of timeout
 * or not
*/
static int readSizeFlag=0;

static int ctrlFlag=0, checkZoomFlag;

static uint64_t hexcol=DEFAULT;   // default shell color

static int isPrintable(unsigned char c);
static int isVerticalArrow(unsigned char c);

static char inputs[MAX_INPUTS_STORE][MAXBUFLEN];
static int inputIndex = 0;

void reverse(char *str) {
    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

char * itoa(int num, char * str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
    }
    else {
        while (num != 0) {
            str[i++] = num % 10 + '0';
            num /= 10;
        }
    }
    str[i] = 0;
    reverse(str);
    return str;
}

char * itoaHex(uint64_t num, char * str) {
    int i = 0;
    while (num != 0) {
        int r = num % 16;
        str[i++] = (r < 10) ? (r + '0') : (r - 10 + 'A');
        num /= 16;
    }
    str[i] = 0;
    reverse(str);
    return str;
}

unsigned char getchar(void){
    unsigned char read=0;
    readSizeFlag=sysReadScreen(STDIN, &read, 1);
    return read;
}

unsigned char putchar(unsigned char c){
    sysWriteScreen(STDOUT, &c, 1, hexcol);
    return c;
}

int printColor(char* str, uint64_t hexColor){
    hexcol=hexColor;
    int i;
    for(i=0;str[i]!='\0';i++){
        putchar(str[i]);
    }
    hexcol=DEFAULT;
    return i;
}

int print(char * str){
    return printColor(str, DEFAULT);
}


int printf(const char *fmt, ...) {
    int count = 0;
    va_list args;
    va_start(args, fmt);

    // Buffer to store the integer to string conversion
    char buffer[PRINTF_BUFFER_SIZE];
    char *str;
    int num;
    uint64_t hex;

    for (int i=0; fmt[i] != 0; i++) {
        if (fmt[i] == '%') {
            switch (fmt[++i]) {
                // String
                case 's':
                    str = va_arg(args, char *);
                    for (int j=0; str[j] != '\0'; j++) {
                        putchar(str[j]);
                        count++;
                    }
                    break;
                // Integer
                case 'd':
                    num = va_arg(args, int);
                    itoa(num, buffer);
                    for (int j=0; buffer[j] != '\0'; j++) {
                        putchar(buffer[j]);
                        count++;
                    }
                    break;
                // Character
                case 'c':
                    putchar(va_arg(args, int));
                    count++;
                    break;
                case 'x':
                    hex = va_arg(args, uint64_t);
                    itoaHex(hex, buffer);
                    putchar('0');
                    putchar('x');
                    count += 2;
                    int digits = 16 - strlen(buffer);
                    for (int j = 0; j < digits; j++) {
                        putchar('0');
                        count++;
                    }
                    for (int j = 0; buffer[j] != '\0'; j++) {
                        putchar(buffer[j]);
                        count++;
                    }
                    break;
                // No special format found
                default:
                    putchar(fmt[i]);
                    count++;
                    break;
            }
        }
        else {
            // No special format
            putchar(fmt[i]);
            count++;
        }
    }
    va_end(args);
    return count;
}

int scanf(char * buffer, int size){
    if(size==0)
        return 0;
    unsigned char read=0;
    int readSize=0, printedSize=0;
    while(read!='\n'){
        ctrlFlag = sysCtrlPressed();
        read=getchar();
        if(!readSizeFlag)
            continue;
        
        if(ctrlFlag){
            if(read == 'i'){
                checkZoomFlag = incTextSize();
                if(checkZoomFlag){
                    print("   Maximum Size Reached.");
                    sysHideCursor();
                    sysPrintCursor();
                    sysSleep(0, 15);
                    for(int i=0; i<strlen("   Maximum Size Reached."); i++){
                        putchar('\b');
                    }
                    sysShowCursor();
                    sysPrintCursor();
                }
            }
            else if(read == 'o'){
                checkZoomFlag = decTextSize();
                if(checkZoomFlag){
                    print("   Minimum Size Reached.");
                    sysHideCursor();
                    sysPrintCursor();
                    sysSleep(0,15);
                    for(int i=0; i<strlen("   Minimum Size Reached."); i++){
                        putchar('\b');
                    }
                    sysShowCursor();
                    sysPrintCursor();
                }
            }
        }
        else{
            if(read=='\n'){
                buffer[readSize]=0;
                if (readSize > 0) strcpy(inputs[inputIndex++ % MAX_INPUTS_STORE], buffer);
                putchar(read);  // Newline
            }
            else if(read=='\b' && readSize!=0){
                if(readSize>=printedSize)
                    readSize--;
                    buffer[readSize]=0;
                putchar(read);  // Backspace
                printedSize--;
            }
            else if (isVerticalArrow(read)) {
                if (read == sysUpArrowValue()) {
                    if (inputIndex > 0) {
                        inputIndex--;
                    }
                } else {
                    if (inputIndex < MAX_INPUTS_STORE) {
                        inputIndex++;
                    }
                }
                strcpy(buffer, inputs[inputIndex % MAX_INPUTS_STORE]);
                int commandLen = strlen(buffer);
                for(int i=0; i<printedSize; i++)
                    putchar('\b');
                printedSize = commandLen;
                readSize = commandLen;
                print(buffer);
            }
            else if(isPrintable(read)){       // Printable Character
                if(readSize!=size)
                    buffer[readSize++]=read;
                putchar(read);
                printedSize++;
            }
        }
    }
    return readSize;
}

static int isPrintable(unsigned char c){
    return (c>=' ' && c<='~');
}

static int isVerticalArrow(unsigned char c) {
    return c == sysUpArrowValue() || c == sysDownArrowValue();
}