#include <exceptions.h>
#include <kernel_syscalls.h>
#include <videoDriver.h>

static char * zero_division_msg = "Zero division exception";

static void print(uint8_t * message, uint8_t cant_chars_message);

extern uint64_t exception_regs[REGS_LEN];
extern void _sys_get_registers();

void exceptionDispatcher(uint64_t exception) {
  uint8_t * message;
	uint8_t message_cant_chars = 1;
	if ( exception == 0 ) {
		message = ( uint8_t * ) "Zero div exception\n\n";
		message_cant_chars = 20;
	}

	if ( exception == 1 ) {
		message = ( uint8_t * ) "Invalid opcode\n\n";
		message_cant_chars = 16;
	}

	print( message, message_cant_chars );
  return;
}

static void print(uint8_t * message, uint8_t cant_chars_message) {
	const char * regs[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rip", "rflags"};
  sys_write(STDOUT, ( char * ) message, cant_chars_message , 0x00FFFFFF);
	for ( int i = 0; i < REGS_LEN; i++ ) {
    sys_write(STDOUT, regs[i], 10 , 0x00FFFFFF);
    sys_write(STDOUT, ": 0x", 4 , 0x00FFFFFF);
    sys_write(STDOUT, (char *) num_to_string(exception_regs[i], 16), 10 , 0x00FFFFFF);
    sys_jump_line();
	}
}