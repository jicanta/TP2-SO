#include <stdint.h>
#include <defs.h>
#include <videoDriver.h>
#include "include/time.h"
#include <keyboard.h>
#include <interrupts.h>
#include <lib.h>
#include <syscallHandle.h>
#include "speaker.h"
#include "fonts.h"
#include "process.h"
#include "scheduler.h"

#define HANDLER_SIZE 45

static int (*syscallHandlers[])()={
    read, write, printRegs, incSize, decSize, getZoomLevel, setZoomLevel, upArrowValue, leftArrowValue, downArrowValue,
    rightArrowValue, clearScreen, printSquare, printRect, setCursor, sound, msSleep, hideCursor,
    showCursor, printCursor, getCurrentSeconds, getCurrentMinutes, getCurrentHours, getCurrentDay,
    getCurrentMonth, getCurrentYear, isctrlPressed, cleanKbBuffer, NULL, NULL, processCreate, getProcesspid, getProcessParentpid, getPs,
    freePs, wait, kill, nice, block, NULL, NULL, NULL, NULL, NULL, NULL,
};

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax){         
    // int handlerSize = sizeof(syscallHandlers)/sizeof(syscallHandlers[0]);

    if(rax < 0 || rax > HANDLER_SIZE)
        return -1;

    return syscallHandlers[rax](rdi,rsi,rdx,r10,r8);
}

int read(uint64_t fd, char * buf, uint64_t count) {
    if(fd!=STDIN) {   // Only can read from standard input
        return 0;
    }
    uint64_t sizeRead=0;
    unsigned char lastRead='\0';
    while(sizeRead!=count && !kbisBufferEmpty()){
            lastRead = kbreadBuf();
            buf[sizeRead++] = lastRead;
    }
    return sizeRead == count? count : sizeRead;    // If we return sizeRead-1 it means we stopped at '\n'
}

int write(uint64_t fd, char * buf, uint64_t count, uint64_t hexColor){
    if(fd != STDOUT)  // Only can write in STDOUT
        return 0;
    int i;
    char toPrint[2]={0,0};
    for(i=0; i<count; i++){
        toPrint[0]=buf[i];
        vdPrint(toPrint, hexColor);
    }
    return i;
}

int incSize(){
    int zoomFail = sizeUp();
    if(!zoomFail)
        vdResize();
    return zoomFail;
}

int decSize(){
    int zoomFail = sizeDown();
    if(!zoomFail)
        vdResize();
    return zoomFail;
}

int getZoomLevel(){
    return getZoom();
}

int setZoomLevel(int zoomLevel) {
    return setZoom(zoomLevel);
}

int upArrowValue() {    
    return kbUpArrowValue();
}

int leftArrowValue() {
    return kbLeftArrowValue();
}

int downArrowValue() {
    return kbDownArrowValue();
}

int rightArrowValue() {
    return kbRightArrowValue();
}

int clearScreen() {
    vdClearScreen();
    vdClearBuffer();
    return 0;
}

int printRect(int x,int y,int base,int height,uint32_t hexColor) {
    vdPrintRect(x,y,base,height,hexColor);
    return 0;
}

int printSquare (int x, int y,int side, uint32_t hexColor){
    vdPrintSquare(x,y,side, hexColor);
    return 0;
}

int setCursor(uint64_t x, uint64_t y) {
    vdSetCursor(x, y);
    return 0;
}

/** registers are stored in a buffer of 18 qwords in the next order:
 *  RIP RFLAGS RSP RAX RBX RCX RDX RSI RDI RBP R8 R9 R10 R11 R12 R13 R14 R15
 */
int printRegs(){
    return regPrinting();
}

int sound(uint64_t ms, uint64_t freq){
    beep(ms, freq);
    return 0;
}

// rdi = seconds, rsi = ms
int msSleep(uint64_t secs, uint64_t ticks){
    if(secs < 0 || ticks < 0)
        return -1;
    int secondsToTicks = secs*18, msToTicks=ticks;
    int totalTicks = secondsToTicks + msToTicks;
    sleep(totalTicks);
    return 0;
}

int hideCursor(){
    vdSetCursorColor(0x00000000);
    return 0;
}

int showCursor(){
    vdSetCursorColor(0x00F0F0F0);
    return 0;
}

int printCursor(){
    vdPrintCursor();
    return 0;
}

int getCurrentSeconds(void){
    return rtc_get_seconds();
}

int getCurrentMinutes(void){
    return rtc_get_minutes();
}

int getCurrentHours(void){
    return rtc_get_hour();
}

int getCurrentDay(void){
    return rtc_get_day();
}

int getCurrentMonth(void){
    return rtc_get_month();
}

int getCurrentYear(void){
    return rtc_get_year();
}

int isctrlPressed(){
    return kbctrlPressed();
}

int cleanKbBuffer(){
    kbcleanBuffer();
    return 0;
}

PID processCreate(creationParameters *params){
    if(params == NULL)
        return -1;
    return createProcess(params);
}

PID getProcesspid(){
    return getpid();
}   

PID getProcessParentpid(){
    return getppid();
}

Process *getPs(){

    return getProcessesInformation();

}

void freePs(Process *processesInfo)
{
    freeProcessesInformation(processesInfo);
}

void wait(PID pidToWait, int *wstatus)
{
    waitProcess(pidToWait, wstatus);
}

int killProcess(PID pid)
{
    kill(pid);
}

int nice(PID pid, Priority newP){
    if(checkPriority(newP) && pid > 0 && pid <= MAX_PID){
        changeProccessPriority(pid, newP);
    }
    else{
        return -1;
    }
}

int block(PID pid){
    return blockProcess(pid);
}

int yield(void)
{
    setYield();
    forceSwitchContent();
    return 0;
}