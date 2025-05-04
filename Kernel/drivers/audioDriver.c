#include <audioDriver.h>

extern void _outb(uint16_t port, uint8_t value);
extern uint8_t _inb(uint16_t port);

void playSound(uint32_t f) {
    uint32_t div = PIT_FREQUENCY / f;

    _outb(0x43, 0xB6); // Channel 2: 0x42, square wave generator
    _outb(0x42, (uint8_t) (div & 0xFF));
    _outb(0x42, (uint8_t) ((div >> 8) & 0xFF));

    uint8_t aux = _inb(0x61);
    if (aux != (aux | 3)) {
        _outb(0x61, aux | 3);
    }
    return;
    
}

void stopSound() {
    uint8_t aux = _inb(0x61) & 0xFC;
    _outb(0x61, aux);
    return;
}