#include "interrupts.h"
#include "ia32-interrupts.h"
#include "keyboard.h"

static const int IDT_SIZE = 256;

struct IDT_entry {
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

extern "C" void load_idt(uint32_t *idt_ptr);

namespace kernel
{

void register_callback(uint32_t callback_function_pointer, uint8_t irq)
{
	static const uint8_t KERNEL_CODE_SEGMENT_OFFSET = 0x08;
	static const uint8_t INTERRUPT_GATE = 0x8e;

	uint8_t vector_index = IRQ_0_VECTOR_START + irq;
	IDT_entry *entry = IDT + vector_index;

	/* populate IDT entry of keyboard's interrupt */
	entry->offset_lowerbits = callback_function_pointer & 0xffff;
	entry->selector = KERNEL_CODE_SEGMENT_OFFSET;
	entry->zero = 0;
	entry->type_attr = INTERRUPT_GATE;
	entry->offset_higherbits = (callback_function_pointer & 0xffff0000) >> 16;

	// prepare address for
	uint32_t idt_address = (uint32_t)IDT;
	uint32_t idt_ptr[2];
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	// fill the IDT descriptor
	load_idt(idt_ptr);

	// enable the irq
	uint8_t pic_addr = (irq < 8) ? PIC1_DATA : PIC2_DATA;
	uint8_t mask = inb(pic_addr);
	mask &= ~(1 << irq); // Clear bit number irq
	outb(pic_addr, mask);
}

extern "C" void keyboard_handler(void)
{
	kernel::PIC_sendEOI(KEYBOARD_IRQ);
	static const uint8_t KEYBOARD_DATA_PORT = 0x60;
	static const uint8_t KEYBOARD_STATUS_PORT = 0x64;
	uint8_t status = kernel::inb(KEYBOARD_STATUS_PORT);
	if (status & 0x01)
	{
		uint8_t scancode = kernel::inb(KEYBOARD_DATA_PORT);
		keyboard::handle_keyboard_scancode(scancode);
	}
}

extern "C" void keyboard_handler_wrapper(void);

void initialize_IDT()
{
	register_callback((uint32_t)keyboard_handler_wrapper, KEYBOARD_IRQ);
}

} // namespace
