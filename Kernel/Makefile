include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_MEM=$(wildcard memory/*.c)
SOURCES_DS=$(wildcard dataStructures/*.c)
SOURCES_SCHED=$(wildcard scheduler/*.c)
SOURCES_PROC=$(wildcard process/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_MEM=$(SOURCES_MEM:.c=.o)
OBJECTS_DS=$(SOURCES_DS:.c=.o)
OBJECTS_SCHED=$(SOURCES_SCHED:.c=.o)
OBJECTS_PROC=$(SOURCES_PROC:.c=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
OBJDIR=obj
STATICLIBS=

all: $(KERNEL) move_objects

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_MEM) $(OBJECTS_DS) $(OBJECTS_SCHED) $(OBJECTS_PROC)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_MEM) $(OBJECTS_DS) $(OBJECTS_SCHED) $(OBJECTS_PROC) $(STATICLICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

$(OBJDIR):
	mkdir -p $(OBJDIR)
	chmod 777 $(OBJDIR)


move_objects: $(OBJDIR) create_dirs
	mv *.o $(OBJDIR) 
	mv asm/*.o $(OBJDIR) 
	mv memory/*.o $(OBJDIR)
	mv dataStructures/*.o $(OBJDIR) 
	mv scheduler/*.o $(OBJDIR) 
	mv process/*.o $(OBJDIR)

clean:
	rm -rf $(OBJDIR) *.bin

.PHONY: all clean create_dirs
