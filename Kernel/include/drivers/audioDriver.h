#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#define PIT_FREQUENCY 1193182
#include <stdint.h>

/**
 * @brief Emite un sonido de determinada frecuencia.
 *
 * @param f frecuencia de emisión.
 * 
 */
void playSound(uint32_t f);

void stopSound();

#endif