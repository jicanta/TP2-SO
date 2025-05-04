#include <drivers/time.h>

static unsigned long ticks = 0;
static uint16_t frequency = 18;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

uint64_t ms_elapsed() {
    return ticks * 55;
}

void setup_timer(uint16_t frec){
	uint16_t divisor = 1193180 / frec;
	_outb(0x43, 0x36);
	_outb(0x40, divisor & 0xFF);
	_outb(0x40, divisor >> 8);
	frequency = frec;
}
