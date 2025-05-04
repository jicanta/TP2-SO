include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_LIB=$(wildcard lib/*.c)
SOURCES_CLIB=$(wildcard clib/*.c)
SOURCES_SHELL=$(wildcard shell/*.c)
SOURCES_COLOR=$(wildcard color/*.c)
SOURCES_COMANDS=$(wildcard shell/comands/*.c)
SOURCES_MODULES_SNAKE=$(wildcard modules/snake/*.c)


OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_LIB=$(SOURCES_LIB:.c=.o)
OBJECTS_CLIB=$(SOURCES_CLIB:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_COLOR=$(SOURCES_COLOR:.c=.o)
OBJECTS_COMANDS=$(SOURCES_COMANDS:.c=.o)
OBJECTS_MODULE_SNAKE=$(SOURCES_MODULES_SNAKE:.c=.o)

LOADERSRC=_loader.c

LOADEROBJECT=$(LOADERSRC:.c=.o)
STATICLIBS=

INCLUDE_DIRS=$(shell find ./include -type d)
INCLUDE_FLAGS=$(foreach dir,$(INCLUDE_DIRS),-I$(dir))

all: $(MODULE)

$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_LIB) $(OBJECTS_CLIB) $(OBJECTS_SHELL) $(OBJECTS_COLOR) $(OBJECTS_COMANDS) $(OBJECTS_MODULE_SNAKE)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_LIB) $(OBJECTS_CLIB) $(STATICLIBS) $(OBJECTS_SHELL) $(OBJECTS_COLOR) $(OBJECTS_COMANDS) $(OBJECTS_MODULE_SNAKE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(GCC) $(GCCFLAGS) $(LOADERSRC) -c -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o

.PHONY: all clean print