#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#define SHIFT_VALUES 2
#define MAX_SCANCODE 69

#include <stdint.h>

void updateBuffer(void);
void kbcleanBuffer(void);
uint8_t getKey(void);
uint8_t kbUpArrowValue(void);
uint8_t kbLeftArrowValue(void);
uint8_t kbDownArrowValue(void);
uint8_t kbRightArrowValue(void);

int kbisBufferEmpty();
unsigned char kbreadBuf(void);
int kbctrlPressed(void);

static unsigned char scancodesChars[SHIFT_VALUES][MAX_SCANCODE] = {
    {
    0,'\x1B', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    
    '\x11', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',       
    '\x12', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\x14',   
    '*', '\x10', ' ', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89'},
    {0,'\x1B', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',    
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',    
    '\x11', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',        
    '\x12', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\x14',      
    '*', '\x10', ' ', '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89'}
};

enum SPECIALCHARS {
 CONTROL           = 0x1d,
 CONTROL_RELEASED = 0x9D,
 L_SHIFT_PRESS     = 0x2a, 
 L_SHIFT_RELEASE   = 0xaa, 
 R_SHIFT_PRESS     = 0x36,
 R_SHIFT_RELEASE   = 0xB6, 
 L_ALT             = 0x38,
 UP_ARROW          = 0x48,
 LEFT_ARROW        = 0x4B,
 DOWN_ARROW        = 0x50,
 RIGHT_ARROW       = 0x4D,
};

#endif