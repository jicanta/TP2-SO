include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_MEM=$(wildcard memory/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_MEM=$(SOURCES_MEM:.c=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
OBJDIR=obj
STATICLIBS=

all: $(KERNEL) move_objects

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_MEM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_MEM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	chmod 777 $(OBJDIR) 

move_objects: $(OBJDIR)
	mv *.o $(OBJDIR)
	mv asm/*.o $(OBJDIR)
	mv memory/*.o $(OBJDIR)
clean:
	rm -rf $(OBJDIR) *.bin

.PHONY: all clean
