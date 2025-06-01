#include <videoDriver.h>
#include <defs.h>
#include <fonts.h>
#include <lib.h>
#include <logo.h>

#define MAXCHARSINSCREEN 10880	// chars per row * chars per column (with minimum size)
#define GREY 0x00F0F0F0
#define BLACK 0x00000000

typedef struct vbe_mode_info_structure * VBEInfoPtr;
VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;
Cursor cursor = {0, 0,GREY};
uint8_t * framebuffer;
uint16_t widthScreen;
uint16_t heightScreen;
uint16_t pitch;
uint8_t  bytesPerPixel;
static uint32_t fgColor = GREY;
const uint32_t bgColor = BLACK;
char charsInScreen[MAXCHARSINSCREEN];
uint32_t colorsInScreen[MAXCHARSINSCREEN];
static int index;
static int scrollUpdateBuffer;

void initializeVideoDriver(){
	framebuffer = (uint8_t *) (uintptr_t)VBE_mode_info->framebuffer;
	widthScreen = 	VBE_mode_info->width;
	heightScreen = VBE_mode_info->height;
	pitch = VBE_mode_info->pitch;
	bytesPerPixel = VBE_mode_info->bpp/BYTE_LENGHT;
	memset(charsInScreen,' ',MAXCHARSINSCREEN);
	memset(colorsInScreen,0,4*MAXCHARSINSCREEN);
	scrollUpdateBuffer = 1;
	index = 0;
}

void vdPrintLine(uint64_t offset,uint32_t fgColor,uint32_t bgColor ,uint8_t mask,int limit,int step){
	uint64_t off = offset;
	uint32_t hexColor;
	for(int i=0; i < limit; i++,off+=step){
		if (mask & (1 << (BYTE_LENGHT - 1 - i))) {
			hexColor = fgColor;
		} 
		else{
			hexColor = bgColor;
		}
		vdPutPixel(off,hexColor);
	}
}

void vdSetCursorColor(uint32_t hexcolor){
	cursor.color = hexcolor;
}

void vdUpdateCursor(int x, int y){
	int offsetX = x * bytesPerPixel * (getCurrentFont().size.realWidth);
	int offsetY = y * pitch * (getCurrentFont().size.height); 

	if(cursor.posY + offsetY >= 0){
		if (cursor.posX + offsetX >= widthScreen*bytesPerPixel)
		{
			offsetX = 0;
			cursor.posX = 0;
			offsetY += (pitch * getCurrentFont().size.height);
		}
		else if(cursor.posX + offsetX < 0){
			offsetX = 0;
			cursor.posX = (widthScreen - getCurrentFont().size.realWidth) * bytesPerPixel;
			offsetY -= pitch * getCurrentFont().size.height;
		}

		if (cursor.posY + offsetY >= heightScreen * pitch)
		{
			vdScroll(1);
			offsetY += -pitch * getCurrentFont().size.height;
			
		}
		else if (cursor.posY + offsetY < 0)
		{
			offsetY = 0;
		}
		cursor.posX += offsetX;
		cursor.posY += offsetY;
	}

}

void vdPrintCursor(){
	if (cursor.posX + (getCurrentFont().size.realWidth*bytesPerPixel) >= widthScreen*bytesPerPixel)
	{
		vdUpdateCursor(1,0);
	}
	if (cursor.posY + (getCurrentFont().size.height*pitch) >= heightScreen*pitch)
	{
		vdScroll(1);
		cursor.posY -= pitch *getCurrentFont().size.height;
	}
	vdPrintRect(cursor.posX/bytesPerPixel,cursor.posY/pitch,getCurrentFont().size.realWidth,getCurrentFont().size.height,cursor.color);
}

void vdSetCursorByPixel(int x, int y){
	int posX = x * bytesPerPixel;
	int posY = y * pitch;
	if(posX < widthScreen*bytesPerPixel && posX >= 0 && posY >= 0 && posY < pitch * heightScreen){
		cursor.posX = posX;
		cursor.posY = posY;
	}
}

void vdSetCursor(int x, int y){
	int posX = x * bytesPerPixel * getCurrentFont().size.realWidth;
	int posY = y * pitch * getCurrentFont().size.height;
	if(posX < widthScreen*bytesPerPixel && posX >= 0 && posY >= 0 && posY < pitch * heightScreen){
		cursor.posX = posX;
		cursor.posY = posY;
	}
}

void vdNewLine(){
	vdPrintRect(cursor.posX/bytesPerPixel,cursor.posY/pitch,getCurrentFont().size.realWidth,getCurrentFont().size.height,BLACK);
	cursor.posX = 0;
	vdUpdateCursor(0,1);
	vdPrintCursor();
}

void vdPutPixel(uint64_t offset,uint32_t hexColor){
	framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> BYTE_LENGHT) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> TWO_BYTE_LENGHT) & 0xFF;
}

void vdPrintChar(unsigned char c) {
	FontBitmap fontBitMap = getCurrentFont();
	unsigned const char* bitmap = fontBitMap.bitmap;
	int width = fontBitMap.size.width;
	int height = fontBitMap.size.height;

	uint64_t offset = cursor.posX + cursor.posY;

	if (width == BYTE_LENGHT) {
		
		for (int y_pos = 0; y_pos < height; y_pos++,offset+=pitch) {
			vdPrintLine(offset,fgColor,bgColor,bitmap[y_pos + (c-31) * height],BYTE_LENGHT,bytesPerPixel);
		}
	}
	else {
		for (int y_pos = 0, pY = 0; y_pos < height*2; y_pos+=2, pY++,offset+=VBE_mode_info->pitch) {
			vdPrintLine(offset,fgColor,bgColor,bitmap[y_pos + (c-31) * height*2],BYTE_LENGHT,bytesPerPixel);
			vdPrintLine(offset+(width*bytesPerPixel/2),fgColor,bgColor,bitmap[y_pos + (c-31) * height*2 + 1],BYTE_LENGHT,bytesPerPixel);
		}
	}
	vdUpdateCursor(1,0);
	vdPrintCursor();
}

void vdPrint(char *characters,uint32_t hexColor){
	fgColor = hexColor;
	for(int i=0;characters[i] != 0;i++){
		char c = characters[i];

		if(c == '\n'){
			colorsInScreen[index] = BLACK;
			charsInScreen[index++] = '\n';
			vdNewLine();
		}

		else if(c == '\b'){
			vdDeleteChar();
			if(index != 0)
				--index;
			colorsInScreen[index] = BLACK;
			charsInScreen[index] = ' ';
		}
		else{
		vdPrintChar(c);
		colorsInScreen[index] = hexColor;
		charsInScreen[index++] = c;
		}
	}
}

void vdPrintInt(int number,uint32_t hexColor){
	char buffer[12];
	int i = 0;
	if(number == 0){
		buffer[i++] = '0';
	}
	else{
		while(number != 0){
			buffer[i++] = (number % 10) + '0';
			number /= 10;
		}
	}
	for(int j = i - 1;j >= 0;j--){
		vdPrintChar(buffer[j]);
	}
}

void vdDeleteChar(){
	vdPrintRect(cursor.posX/bytesPerPixel,cursor.posY/pitch,getCurrentFont().size.realWidth,getCurrentFont().size.height,BLACK);
	vdUpdateCursor(-1,0);
	vdPrintRect(cursor.posX/bytesPerPixel,cursor.posY/pitch,getCurrentFont().size.realWidth,getCurrentFont().size.height,BLACK);
	vdPrintCursor();
}

void vdResize(){
	vdClearScreen();
	scrollUpdateBuffer = 0;
	char c;
	for(int j=0;j<index;j++){
		c = charsInScreen[j];
		if(c == '\n'){
			vdNewLine();
		}
		else{
			fgColor = colorsInScreen[j];	
			vdPrintChar(c);
			}	
	}
	scrollUpdateBuffer = 1;
}

void vdClearScreen(){
	memset(framebuffer,0,bytesPerPixel * heightScreen * widthScreen);
	vdSetCursor(0,0);
}

void vdClearBuffer(){
	memset(charsInScreen,' ',MAXCHARSINSCREEN);
	memset(colorsInScreen,0,4*MAXCHARSINSCREEN);
	index = 0;
}

void updateCharsInScreen(int lines){
	for(int i=lines * (widthScreen/getCurrentFont().size.realWidth),j=0;i<MAXCHARSINSCREEN;i++){
		colorsInScreen[j] = colorsInScreen[i];
		charsInScreen[j++] = charsInScreen[i];
	}
}

void vdScroll(int lines) {
    int fontHeight = getCurrentFont().size.height;
    int offset = lines * fontHeight * widthScreen * bytesPerPixel;
    int frameSize = bytesPerPixel * widthScreen * (heightScreen - lines * (fontHeight));
    int clearSize = bytesPerPixel * widthScreen * lines * fontHeight;
    memcpy(framebuffer, framebuffer + offset, frameSize);
    memset(framebuffer + frameSize, 0, clearSize);
	if(scrollUpdateBuffer == 1 && index >= MAXCHARSINSCREEN){
		if(index - lines * (widthScreen/getCurrentFont().size.realWidth) >=0){
			index -= lines * (widthScreen/getCurrentFont().size.realWidth);
		}
		updateCharsInScreen(lines);
	}
}

void vdPrintRect(int x,int y,int base, int height, uint32_t hexColor){
	uint64_t offsetX = x * bytesPerPixel;
	uint64_t posX = offsetX;
	uint64_t offsetY = y * pitch;
	for(int y=0;y < height;y++,offsetY += pitch) {
		for(int x = 0,offsetX = posX; x < base;x++, offsetX += bytesPerPixel)
			vdPutPixel(offsetX + offsetY,hexColor);
	}
}

void vdPrintSquare(int x, int y,int side,uint32_t hexColor){
	vdPrintRect(x,y,side,side,hexColor);
}

#define BOOT_BG_TOP    0x0018181F   /* gris azulado oscuro (arriba) */
#define BOOT_BG_BOTTOM 0x00404048   /* gris azulado claro  (abajo)  */
#define BOOT_FG_COLOR  0x00FFFFFF   /* texto/blanco                */
#define BOOT_BAR_BG    0x00202020
#define BOOT_BAR_FG    0x0080C0FF

static inline uint32_t lerpColor(uint32_t a, uint32_t b, float t) {
    uint8_t ab = a & 0xFF,   ag = (a >> 8) & 0xFF,   ar = (a >> 16) & 0xFF;
    uint8_t bb = b & 0xFF,   bg = (b >> 8) & 0xFF,   br = (b >> 16) & 0xFF;
    uint8_t rb = ab + (uint8_t)((bb - ab) * t);
    uint8_t rg = ag + (uint8_t)((bg - ag) * t);
    uint8_t rr = ar + (uint8_t)((br - ar) * t);
    return (rr << 16) | (rg << 8) | rb;
}

/* Fondo vertical en degradé */
static void drawGradient(void) {
    for (int y = 0; y < heightScreen; y++) {
        float t = (float)y / (float)heightScreen;
        uint32_t c = lerpColor(BOOT_BG_TOP, BOOT_BG_BOTTOM, t);
        vdPrintRect(0, y, widthScreen, 1, c);
    }
}

/* Barra de carga: percent ∈ [0,100] */
static void drawProgressBar(int percent) {
    int barW = widthScreen * 3 / 5;
    int barH = getCurrentFont().size.height;
    int barX = (widthScreen - barW) / 2;
    int barY = heightScreen * 3 / 4;

    /* marco */
    vdPrintRect(barX, barY, barW, barH, BOOT_BAR_BG);
    /* relleno */
    int fill = (barW - 2) * percent / 100;
    vdPrintRect(barX + 1, barY + 1, fill, barH - 2, BOOT_BAR_FG);
}

static int asciiLen(const char *s){
    int n = 0;
    while (s[n] && (unsigned char)s[n] < 0x80) n++;
    return n;
}


/* Texto centrado (1 línea) */
static void centerPrint(const char *msg, uint32_t color, int y) {
    int len = asciiLen(msg);
    int x = (widthScreen / 2) - (len * getCurrentFont().size.realWidth) / 2;
    vdSetCursor(x / getCurrentFont().size.realWidth, y);
    vdPrint((char *)msg, color);
}

/* API pública */
void vdShowBootScreen(void) {

    scrollUpdateBuffer = 0;   /* deshabilitamos scroll/backup temporal */

    drawGradient();

	int scale = (widthScreen / 3) / STITCH_W;
    if (scale < 1) scale = 1;           /* evita 0 si la pantalla es chica */

    int logoW = STITCH_W * scale;
    int logoH = STITCH_H * scale;

    int logoX = (widthScreen  - logoW) / 2;
    int logoY = (heightScreen / 3) - (logoH / 2);

    vdDrawBitmap32Scaled(logoX, logoY,
                         STITCH_W, STITCH_H,
                         STITCH_PIXELS,
                         scale);

    centerPrint("LA BANDA OS", BOOT_FG_COLOR, heightScreen / 2 - 2);
    centerPrint("loading kernel...", BOOT_FG_COLOR, heightScreen / 2);

    /* animación sencilla de 0 a 100 % */
    for (int p = 0; p <= 100; p += 4) {
        drawProgressBar(p);
        /* delay ~15 ms aprox. usando el timer PIT */
        _hlt();               /* cede CPU hasta la próxima interrupción */
    }

    /* Pequeña pausa antes de borrar el boot-screen */
    for (int i = 0; i < 30; i++) _hlt();

    scrollUpdateBuffer = 1;
    vdClearScreen();
}

void vdDrawBitmap32(int dstX, int dstY, int w, int h, const uint32_t *pixels) {
    /* clip simple */
    if (dstX < 0 || dstY < 0 || dstX + w > widthScreen || dstY + h > heightScreen)
        return;

    uint8_t *row = framebuffer + dstY * pitch + dstX * bytesPerPixel;
    for (int y = 0; y < h; y++, row += pitch) {
        uint8_t *p = row;
        for (int x = 0; x < w; x++, p += bytesPerPixel) {
            uint32_t c = pixels[y * w + x];
            /* salta el color 0 como “transparente”         */
            if (c) {               /* 0x00RRGGBB  (alpha-less) */
                p[0] =  c        & 0xFF;
                p[1] = (c >>  8) & 0xFF;
                p[2] = (c >> 16) & 0xFF;
            }
        }
    }
}

void vdDrawBitmap32Scaled(int dstX, int dstY, int w, int h, const uint32_t *pixels, int scale) {
    if (scale <= 0) scale = 1;

    int outW = w * scale;
    int outH = h * scale;

    /* clipping rudimentario */
    if (dstX < 0 || dstY < 0 ||
        dstX + outW > widthScreen || dstY + outH > heightScreen)
        return;

    for (int y = 0; y < h; y++) {
        for (int sy = 0; sy < scale; sy++) {
            /* fila destino en framebuffer */
            uint8_t *row = framebuffer
                         + (dstY + y * scale + sy) * pitch
                         + dstX * bytesPerPixel;

            for (int x = 0; x < w; x++) {
                uint32_t c = pixels[y * w + x];
                for (int sx = 0; sx < scale; sx++, row += bytesPerPixel) {
                    if (c) {                 /* 0 = transparencia */
                        row[0] =  c        & 0xFF;   /* B */
                        row[1] = (c >> 8)  & 0xFF;   /* G */
                        row[2] = (c >> 16) & 0xFF;   /* R */
                    }
                }
            }
        }
    }
}