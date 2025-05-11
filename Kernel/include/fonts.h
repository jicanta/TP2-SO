/***************************************************
  fonts.h
****************************************************/

#include <bytes.h>

#ifndef FONT_H
#define FONT_H

#define FONTS_QUANTITY	7

#define A_FONT			0
#define B_FONT			1
#define C_FONT			2
#define D_FONT			3
#define E_FONT			4
#define F_FONT			5
#define G_FONT			6
#define H_FONT			7
#define I_FONT			8


#define DEFAULT_FONT	D_FONT

typedef struct {
  int realWidth;
	int width;
	int height;
} Size;

typedef struct {
    unsigned const char *bitmap;
    Size size;
} FontBitmap;

typedef struct {
    FontBitmap fonts[FONTS_QUANTITY];
    int currentFontIndex;
} FontManager;

void initFontManager();
int setCurrentFont(int index);
FontBitmap getCurrentFont();
int sizeUp(void);
int sizeDown(void);

int getZoom();
int setZoom(int zoomLevel);

#endif
