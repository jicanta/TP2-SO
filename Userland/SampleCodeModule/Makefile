include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c) ../tests/test_mm.c ../tests/test_processes.c ../tests/test_sync.c ../tests/test_prio.c ../tests/test_util.c
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) $(OBJECTS_ASM) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 -no-pie _loader.c $(SOURCES) $(OBJECTS_ASM) -o ../sampleCodeModule.elf
	
obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf obj

.PHONY: all clean print
