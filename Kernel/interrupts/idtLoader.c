#include <interrupts/idtLoader.h>

static void setup_IDT_entry (int index, uint64_t offset);

#pragma pack(push)
#pragma pack(1)

/* Descriptor de interrupcion */
typedef struct {
  uint16_t offset_l;   // Lower 16 bits of the ISR address (bits 0–15)
  uint16_t selector;   // Code segment selector for the ISR (16 bits)
  uint8_t cero;        // Constant field (IST) and reserved (8 bits, should be 0)
  uint8_t access;      // Type and attribute byte (8 bits)  P | DPL | 0 | Type
  uint16_t offset_m;   // Middle 16 bits of the ISR address (bits 16–31)
  uint32_t offset_h;   // Higher 32 bits of the ISR address (bits 32–63)
  uint32_t other_cero; // Reserved, should be 0 (32 bits)
} DESCR_INT;

#pragma pack(pop)

/* IDT configurada en posicion 0 por el Bootloader */
DESCR_INT * idt = (DESCR_INT *) 0;	// IDT de 255 entradas

void load_idt() {
  _cli();

  setup_IDT_entry(0x20, (uint64_t)&_irq00Handler);
  setup_IDT_entry(0x21, (uint64_t)&_irq01Handler);
  setup_IDT_entry (0x00, (uint64_t)&_exception0Handler);
  setup_IDT_entry(0x06, (uint64_t)&_exception1Handler);

  setup_IDT_entry(0x80, (uint64_t)&_irq80Handler);

	picMasterMask(0xFC);
	picSlaveMask(0xFF);

  _sti();
}

static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = 0;
}