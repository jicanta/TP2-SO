#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6
#include <videoDriver.h>
#include <keyboard.h>
#include <interrupts.h>
#include <fonts.h>
#include <lib.h>
#include <defs.h>

#define ERRORCOL 0x00FF0000
#define COMMENTCOL 0x00FFFFFF

int msSleep(uint64_t rdi, uint64_t rsi);
int read(uint64_t fd, char * buf, uint64_t count);

static void zero_division();
static void invalid_opcode();
int hideCursor();

void exceptionDispatcher(int exception) {
	saveRegs();
	vdClearScreen();
	hideCursor();
	setCurrentFont(DEFAULT_FONT);
	for(int i=0; i<10; i++) vdPrint("\n", COMMENTCOL);
	vdPrint("ERROR: An exception has occurred during runtime. More information:\n\n", ERRORCOL);

	switch (exception){
		case ZERO_EXCEPTION_ID:
			zero_division();
			break;
		case INVALID_OPCODE_ID:
			invalid_opcode();
			break;
	}
	regPrinting();
	vdPrint("\n", COMMENTCOL);
	msSleep(3, 5);
	vdPrint("Press any key to recover...\n",COMMENTCOL);
	int flag=0;
	char key;
	kbcleanBuffer();
	
	while(!flag){
		flag=read(STDIN, &key, 1);
	}
	vdClearScreen();
	vdClearBuffer();
}

static void zero_division() {
	vdPrint("Arithmetic exception: division by zero\n",COMMENTCOL);
}

static void invalid_opcode(){
	vdPrint("Invalid Opcode exception: command not recognised\n",COMMENTCOL);

}