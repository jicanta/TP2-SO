#include <stdint.h>
#include <defs.h>
#include <videoDriver.h>
#include "../include/time.h"
#include <keyboard.h>
#include <interrupts.h>
#include <lib.h>
#include <syscallHandle.h>
#include "speaker.h"
#include "fonts.h"
#include "process.h"
#include "scheduler.h"
#include "memoryManager.h"
#include "fileDescriptors.h"
#include "semManager.h"

#define HANDLER_SIZE 60

static int (*syscallHandlers[])()={
    read, write, printRegs, incSize, decSize, getZoomLevel, setZoomLevel, upArrowValue, leftArrowValue, downArrowValue,
    rightArrowValue, clearScreen, printSquare, printRect, setCursor, sound, msSleep, hideCursor,
    showCursor, printCursor, getCurrentSeconds, getCurrentMinutes, getCurrentHours, getCurrentDay,
    getCurrentMonth, getCurrentYear, isctrlPressed, cleanKbBuffer, (int (*)())myMalloc, (int (*)())myFree, (int (*)())processCreate, (int (*)(void))getProcesspid, (int (*)(void))getProcessParentpid, (int (*)())getPs,
    (int (*)())freePs, (int (*)())wait, (int (*)())kill, (int (*)())nice, (int (*)())block, (int (*)())getMemStatus, yield, dispatchSemOpen, dispatchSemClose, (int (*)(void))dispatchSemWait, (int (*)(void))dispatchSemPost,
    dispatchSemValue,(int (*)(void))dispatchSemDestroy, handleCreatePipe, handleGetFD, handlePrintFD, handlePrintSem
};

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax){         

    if(rax < 0 || rax > HANDLER_SIZE)
        return -1;

    return syscallHandlers[rax](rdi,rsi,rdx,r10,r8);
}

void handlePrintFD(){
    printFD();
}

void handlePrintSem(){
    printSem();
}


int read(uint64_t fd, char * buf, uint64_t count) {

    return readFromFD(fd, buf, count);
    
}

int write(uint64_t fd, char * buf, uint64_t count, uint64_t hexColor){
    
    return writeToFD(fd, buf, count, hexColor);
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

void * myMalloc(int size){
    return allocMemory(size);
}

void myFree(void * ptr){
    freeMemory(ptr);
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
    return kill(pid);
}

int nice(PID pid, Priority newP){
    if(checkPriority(newP) && isValidPID(pid)){
        return changeProccessPriority(pid, newP);
    }
    else{
        return -1;
    }
}

int block(PID pid){

    if(pid == 1 || !isValidPID(pid)) return -1; //El usuario no puede bloquear el proceso init

    return blockProcess(pid);
}

void getMemStatus(MemoryStatus *memStatus){
    getMemoryStatus(memStatus);
}

int yield(void)
{
    setYield();
    forceSwitchContent();
    return 0;
}

int dispatchSemOpen(const char * name, int value){
    int semId;
    
    if (name == NULL){
        return 0;
    }

    if (findSemByName(name) < 0){
        semId = semCreate(name, value);

        if (semId < 0) {
            return -1;
        }

        return semId;
    }
    return semOpen(name);
}

int dispatchSemClose(int semId){
    return semClose(semId);
}

void dispatchSemWait(int semId){
    if (semId < 0 || !semExists(semId)) {
        return;
    }

    semWait(semId);
}

void dispatchSemPost(int semId){
    if (semId < 0 || !semExists(semId)) {
        return;
    }

    semPost(semId);
}

void dispatchSemValue(int semId){
    return semValue(semId);
}

void dispatchSemDestroy(int semId){
    if (semId < 0 || !semExists(semId)) {
        return;
    }

    semDestroy(semId);
}

int handleCreatePipe(int fds[2]) {
    if (fds == NULL) {
        return -1; // Error: fds is NULL
    }

    int result = createPipe(fds);
    if (result < 0) {
        return -1; // Error creating pipe
    }

    return 0; // Success
}

int handleGetFD(int *fds) {
    if (fds == NULL) {
        return -1; // Error: fds is NULL
    }

    return getFileDescriptors(fds);
}