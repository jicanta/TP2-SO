#include <stdint.h>
#include <videoDriver.h>
#include <lib.h>

void saveRegsInBuffer(void);
uint64_t* getRegs(void);
static int backupDone=0;
static uint64_t * registers;

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

void saveRegs(void){
	backupDone=1;
}

static char* hexToString(uint64_t value) {
    static char str[17];
    str[16] = '\0';
    
    char hex[] = "0123456789ABCDEF";
    
    uint64_t mask = 0xF;

    for (int i = 15; i >= 0; --i) {
        int digit = (value >> (i * 4)) & mask;
        str[15 - i] = hex[digit];
    }
    
    return str;
}

int regPrinting(void){	
	if(!backupDone)
		return 1;
	registers=getRegs();
	int count=0;
    char * value;
    char * regFormat[]={"RIP:    ","RFLAGS:    ", "RSP:    ", "RAX:       ", "RBP:    ", "RCX:       ", "RDX:    ", "RSI:       ", "RDI:    ", "RBX:       ", "R8:     ", "R9:        ",
        "R10:    ", "R11:       ", "R12:    ", "R13:       ", "R14:    ", "R15:       "};
    for(int i=0; i<18; i++){
        value=hexToString(registers[i]);
        vdPrint(regFormat[i], 0x00FFFFFF);
        vdPrint("0x", 0x00FFFFFF);
        vdPrint(value, 0x00FFFFFF);
        count++;
        if(count==2){
            vdPrint("\n", 0x00000000);
            count=0;
        }
        else{
            vdPrint("  ", 0x00000000);
        }
    }
    vdPrint("\n", 0x00000000);
	return 0;
}

int strlen(const char *str) {
	int length = 0;
	while (str[length] != '\0') {
		length++;
	}
	return length;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}