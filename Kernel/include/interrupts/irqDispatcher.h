#ifndef IRQ_DISPATCHER_H
#define IRQ_DISPATCHER_H

#include <stdint.h>
#include <drivers/keyboard.h>
#include <drivers/time.h>
#include <drivers/videoDriver.h>

void irqDispatcher(uint64_t irq);

static void int_20();
static void int_21();

#endif