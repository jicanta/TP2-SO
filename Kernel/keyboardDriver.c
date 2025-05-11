#include <keyboard.h>
#include <lib.h>
#include <videoDriver.h>

#define MAXSIZE 128

#define UP_ARROW_VAL        0xE0
#define LEFT_ARROW_VAL      0xE1
#define DOWN_ARROW_VAL      0xE2
#define RIGHT_ARROW_VAL     0xE3

static unsigned char buffer[MAXSIZE] = {0};
static int bufferPos = 0;
static int shift = 0;
static int dataStatus = 0;
static int ctrlPressed=0;

int shiftHandler(uint8_t key){
    switch(key) {
        case L_SHIFT_PRESS:
        case R_SHIFT_PRESS:
            shift = 1;
            return 1; 
            
        case L_SHIFT_RELEASE:
        case R_SHIFT_RELEASE:
            shift = 0;
            return 1;

        default:
            return 0; 
    }
}

uint8_t isArrow (uint8_t key) {
    switch (key)
    {
    case UP_ARROW:
        return UP_ARROW_VAL;
    
    case LEFT_ARROW:
        return LEFT_ARROW_VAL;

    case DOWN_ARROW:
        return DOWN_ARROW_VAL;
    
    case RIGHT_ARROW:
        return RIGHT_ARROW_VAL;

    default:
        return 0;
    }    
}

uint8_t kbUpArrowValue() {
    return UP_ARROW_VAL;
}

uint8_t kbLeftArrowValue() {
    return LEFT_ARROW_VAL;
}

uint8_t kbDownArrowValue() {
    return DOWN_ARROW_VAL;
}

uint8_t kbRightArrowValue() {
    return RIGHT_ARROW_VAL;
}

void updateBuffer() {
    uint8_t scancode = getKey();
    uint8_t arrowValue = isArrow(scancode);

    if(scancode==L_ALT){
        saveRegs();
    }
    else if(scancode==CONTROL){
        ctrlPressed=1;
    }
    else if(scancode==CONTROL_RELEASED){
        ctrlPressed=0;
    }
    else if(arrowValue || (!shiftHandler(scancode) && scancode < MAX_SCANCODE)) { // We add the characters, with their corresponding modification for a shift
        dataStatus = 1;
        char c = (arrowValue != 0)? arrowValue : scancodesChars[shift][scancode];
        buffer[bufferPos++] = c;
        if (bufferPos >= MAXSIZE) {
            bufferPos = 0;
        }
    }
}

int kbctrlPressed(){
    return ctrlPressed;
}

int kbisBufferEmpty(){
    return bufferPos == 0;
}

void kbcleanBuffer(){
    bufferPos = 0;
}

unsigned char kbreadBuf () {
    if(kbisBufferEmpty())
        return 0;
    
    unsigned char ans = buffer[0];
    for(int i=0; i<bufferPos-1; i++)
        buffer[i]=buffer[i+1];

    bufferPos--;
    return ans;
}   