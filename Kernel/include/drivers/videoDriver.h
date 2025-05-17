#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include <drivers/font.h>


void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

void putChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x, uint64_t y);

uint64_t putRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height);

uint64_t clearScreen();

uint16_t getScreenWidth();

uint16_t getScreenHeight();

int getScale();

int setScale(uint8_t new_scale);

void incScale();

void decScale();

#endif