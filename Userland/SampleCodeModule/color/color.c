#include <color.h>

static int textColor = WHITE;
static int bgColor = BLACK;

int getTextColor(){
    return textColor;
}

int getBgColor(){
    return bgColor;
}

void setTextColor(int color){
    textColor = color;
}

void setBgColor(int color){
    bgColor = color;
}