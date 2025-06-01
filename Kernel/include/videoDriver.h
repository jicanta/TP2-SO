#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

typedef struct{
	int posX;
	int posY;
	uint32_t color;
}Cursor;

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated 
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));


void vdPrintChar(unsigned char c);
void vdPutPixel(uint64_t offset,uint32_t hexcolor);
void vdPrint(char *characters,uint32_t hexColor);
void vdPrintInt(int number,uint32_t hexColor);
void vdClearScreen();
void vdScroll(int lines);
void initializeVideoDriver();
void vdSetCursor(int x,int y);
void vdUpdateCursor(int x,int y);
void vdDeleteChar();
void vdNewLine();
void vdClearBuffer();
void vdResize();
void vdPrintRect(int x,int y,int base,int height,uint32_t hexcolor);
void vdPrintSquare(int x, int y,int side,uint32_t hexcolor);
void vdSetCursorByPixel(int x, int y);
void vdSetCursorColor(uint32_t color);
void vdPrintCursor();
void vdShowBootScreen(void);
void vdDrawBitmap32(int dstX, int dstY, int w, int h, const uint32_t *pixels); 
void vdDrawBitmap32Scaled(int x, int y, int w,int h, const uint32_t *pix, int scale);


#endif
