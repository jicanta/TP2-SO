#ifndef MODES_H
#define MODES_H


#include "processStructure.h"

void help(void);
void divByZero(void);
void invalidOp(void);
void zoomin(void);
void zoomout(void);
void time(void);
void date(void);
void clear(void);
void registers(void);
void notFound(char* commandNotFound);
void playEasterEgg();
void printProcessesInformation();
void loop();
void kill(PID pid);
void nice(PID pid, Priority newPriority);
void block(PID pid);
void printMemoryStatus();
void test();

#endif
