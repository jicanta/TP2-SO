// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/time.h"
#include <interrupts.h>
#include <stdint.h>
#include <sys/io.h>

static unsigned long ticks = 0;

void timer_handler() {

	ticks++;
}

int ms_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void sleep(int msToSleep){
	long start = ticks;
	while((ticks-start)<=msToSleep){
		_hlt();
	}
}

void initializeTimer(){
	//uint32_t divisor = 1193180 / 1193;

    // Set the PIT control word for channel 0
    //outb(0x43, 0x36); // 0x36 = 00 11 011 0
                      // 00: Select channel 0
                      // 11: Access mode: lobyte/hibyte
                      // 011: Operating mode: Mode 3 (square wave generator)
                      // 0: Binary mode (16-bit binary)

    // Send the divisor to channel 0 data register
    //outb(0x40, (uint8_t)(divisor & 0xFF));      // Send the low byte
    //outb(0x40, (uint8_t)((divisor >> 8) & 0xFF)); // Send the high byte
}