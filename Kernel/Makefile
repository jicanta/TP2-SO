# ────────────────────────────────────────────────────────────────────
#  Kernel Makefile  –  memoria seleccionable (naive | buddy)
# ────────────────────────────────────────────────────────────────────

include Makefile.inc

### ───── Selección de Memory Manager ───────────────────────────────
# Valores válidos:  naive  |  buddy
MEM_IMPL ?= naive

# Ruta al .c elegido
CHOSEN_MEM_SRC := memory/$(MEM_IMPL)MemoryManager.c
CHOSEN_MEM_OBJ := $(CHOSEN_MEM_SRC:.c=.o)

### ───── Fuentes y objetos generales ───────────────────────────────
SOURCES        := $(wildcard *.c)
SOURCES_ASM    := $(wildcard asm/*.asm)
SOURCES_COLLECTIONS := $(wildcard collections/*.c)
SOURCES_DRIVERS := $(wildcard drivers/*.c)

# Todos los .c de memory/, pero quitamos los dos gestores
SOURCES_MEM_ALL := $(wildcard memory/*.c)
SOURCES_MEM     := $(filter-out \
                     memory/naiveMemoryManager.c \
                     memory/buddyMemoryManager.c, \
                     $(SOURCES_MEM_ALL)) \
                   $(CHOSEN_MEM_SRC)

SOURCES_PROC := $(wildcard process/*.c)


OBJECTS        := $(SOURCES:.c=.o)
OBJECTS_ASM    := $(SOURCES_ASM:.asm=.o)
OBJECTS_MEM    := $(SOURCES_MEM:.c=.o)
OBJECTS_PROC   := $(SOURCES_PROC:.c=.o)
OBJECTS_COLLECTIONS := $(SOURCES_COLLECTIONS:.c=.o)
OBJECTS_DRIVERS := $(SOURCES_DRIVERS:.c=.o)

LOADERSRC      := loader.asm
LOADEROBJECT   := $(LOADERSRC:.asm=.o)

KERNEL         := kernel.bin
OBJDIR         := obj
STATICLIBS     :=

### ───── Reglas principales ────────────────────────────────────────
all: $(KERNEL) move_objects

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) \
           $(OBJECTS_MEM)  $(OBJECTS_PROC) $(OBJECTS_COLLECTIONS) $(OBJECTS_DRIVERS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^

### ───── Compilación C / ASM ───────────────────────────────────────
%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT): $(LOADERSRC)
	$(ASM) $(ASMFLAGS) $< -o $@

### ───── Organización de objetos ───────────────────────────────────
$(OBJDIR):
	mkdir -p $@
	chmod 777 $@

move_objects: $(OBJDIR)
	mv -f *.o $(OBJDIR) 2>/dev/null || true
	mv -f asm/*.o $(OBJDIR) 2>/dev/null || true
	mv -f memory/*.o $(OBJDIR) 2>/dev/null || true

### ───── Targets de conveniencia ───────────────────────────────────
buddy:
	$(MAKE) MEM_IMPL=buddy all

naive:
	$(MAKE) MEM_IMPL=naive all

clean:
	rm -rf $(OBJDIR) *.bin *.elf asm/*.o memory/*.o *.o

.PHONY: all clean buddy naive move_objects
