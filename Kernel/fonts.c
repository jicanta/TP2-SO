// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <fonts.h>
#include <bitmaps.h>

unsigned const char* fonts[] = {fontBitMap_8x12,fontBitMap_8x14,fontBitMap_16x18,fontBitMap_16x22,fontBitMap_16x24,fontBitMap_16x28, fontBitMap_16x32};

Size sizes[7] = {{6,8,12},{8,8,14},{10,16,18},{11,16,22},{12,16,24},{14,16,28}, {16,16,32}};

static FontManager global_font_manager;

void initFontManager() {
    for (int i = 0; i < FONTS_QUANTITY; i++) {
        global_font_manager.fonts[i].bitmap = fonts[i];
        global_font_manager.fonts[i].size.width = sizes[i].width;
        global_font_manager.fonts[i].size.height = sizes[i].height;
        global_font_manager.fonts[i].size.realWidth = sizes[i].realWidth;
    }
	
    global_font_manager.currentFontIndex = DEFAULT_FONT;
}

int setCurrentFont(int index) {
    if (index >= 0 && index < FONTS_QUANTITY) {
        global_font_manager.currentFontIndex = index;
        return 0;
    }
    return 1; 
}

FontBitmap getCurrentFont() {
    return global_font_manager.fonts[global_font_manager.currentFontIndex];
}

int sizeUp(){
    return setCurrentFont(global_font_manager.currentFontIndex+1);
}

int sizeDown(){
    return setCurrentFont(global_font_manager.currentFontIndex-1);
}

int getZoom(){
    return global_font_manager.currentFontIndex;
}

int setZoom(int zoomLevel){
    return setCurrentFont(zoomLevel);
}