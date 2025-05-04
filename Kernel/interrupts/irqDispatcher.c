#include <interrupts/irqDispatcher.h>

extern uint64_t take_snapshot;

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
	}
	return;
}

static void int_20() { // Timer tick (0x20)
	timer_handler();
}

static void int_21() { // Keyboard (0x21)
	take_snapshot = keyboard_handler();
}