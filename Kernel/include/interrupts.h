 /*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idtLoader.h>

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);

void forceSwitchContent();


void _exception0Handler(void);
void _exception6Handler(void);

void _syscallHandler(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

//The CPU execution ends.
void haltcpu(void);

int spinlockAcquire(void *lock);

void spinlockRelease(void *lock);

#endif /* INTERRUPS_H_ */
