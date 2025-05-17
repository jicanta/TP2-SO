#include <drivers/videoDriver.h>

#define DEFAULT_SCALE 1
#define MIN_SCALE 1
#define MAX_SCALE 2

static volatile int scale = DEFAULT_SCALE;


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

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}

static void putScaledPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    for (int i = 0; i < scale; i++) {
        for (int j = 0; j < scale; j++) {
            putPixel(hexColor, x + i, y + j);
        }
    }
}

uint16_t getScreenWidth() {
    return VBE_mode_info->width;
}

uint16_t getScreenHeight() {
    return VBE_mode_info->height;
}

void putChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x, uint64_t y) {
	uint8_t * charBitmap = getFontChar(c);

    if(charBitmap == NULL)
        return;

    for (int i = 0; i < getFontHeight(); i++) {
        for (int j = 0; j < getFontWidth(); j++) {
            if ((charBitmap[i] >> (getFontWidth() - j - 1)) & 1) {
                putScaledPixel(charColor, x, y);
            }
            else {
                putScaledPixel(bgColor, x, y);
            }
            x += scale;
        }
        x -= getFontWidth() * scale;
        y += scale;
    }
}

uint64_t putRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width, uint64_t height) {
    if((x + width) > getScreenWidth() || (y + height) > getScreenHeight())
        return 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            putPixel(hexColor, x + j, y + i);
        }
    }
    return 1;
}

uint64_t clearScreen() {
	return putRectangle(0x00000000, 0, 0, getScreenWidth(), getScreenHeight());
}

int getScale(){
    return scale;
}

int setScale(uint8_t new_scale){
    if (new_scale >= MIN_SCALE && new_scale <= MAX_SCALE) {
        scale = new_scale;
        return 1;
    }
    return 0;
}

void incScale(){
    if (scale < MAX_SCALE)
        scale++;
    return;
}

void decScale(){
    if (scale > MIN_SCALE)
        scale--;
    return;
}