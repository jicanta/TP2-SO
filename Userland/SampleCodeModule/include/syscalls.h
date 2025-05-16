#ifndef __SYSCALLS_H_
#define __SYSCALLS_H_

#include <stdint.h>

typedef struct {
    uint32_t total;  
    uint32_t used;   
    uint32_t free;    
    void    *base;    
    void    *end;     
} MemoryStatus;

int sysWriteScreen(uint64_t fd, unsigned char* buffer, uint64_t len, uint64_t hexColor);
int sysReadScreen(uint64_t fd, unsigned char* buffer, uint64_t len);
void sysSleep(uint64_t secs, uint64_t ms);
int sysClearScreen();
int sysClearKbEntry();

int sysCtrlPressed();

uint8_t sysUpArrowValue();
uint8_t sysLeftArrowValue();
uint8_t sysDownArrowValue();
uint8_t sysRightArrowValue();
int sysPrintRectangle(int x,int y,int base,int height,uint32_t hexColor);
int sysPrintSquare(int x,int y , int side, uint32_t hexColor);
int sysSetCursorPosition(uint64_t x, uint64_t y);
int sysBeepSound(uint64_t ms, uint64_t freq);

int sysGetSecs(void);
int sysGetMins(void);
int sysGetHour(void);

int sysGetDay(void);
int sysGetMonth(void);
int sysGetYear(void);

int sysZoomIn(void);
int sysZoomOut(void);
int sysGetZoomLevel(void);
int sysSetZoomLevel(int zoomLevel);

int sysPrintRegs(void);
int sysPrintCursor();
int sysHideCursor();
int sysShowCursor();

void * sysMalloc(uint32_t size);
void sysFree(void * memorySegment);
void sysGetMemState(MemoryStatus *status);

#endif