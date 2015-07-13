#include "interrupts.h"

static const uint8_t PIC1_COMMAND  = 0x20;   // Master PIC IO base address
static const uint8_t PIC1_DATA     = 0x21;
static const uint8_t PIC2_COMMAND  = 0xA0;   // Slave PIC IO base address
static const uint8_t PIC2_DATA     = 0xA1;

// ICW1 flags
static const uint8_t ICW1_ICW4      = 0x01;		// ICW4 (not) needed
static const uint8_t ICW1_SINGLE    = 0x02;		// Single (cascade) mode
static const uint8_t ICW1_INTERVAL4 = 0x04;		// Call address interval 4 (8)
static const uint8_t ICW1_LEVEL     = 0x08;		// Level triggered (edge) mode
static const uint8_t ICW1_INIT      = 0x10;		// Initialization - required!

// ICW4 flags
static const uint8_t ICW4_8086       = 0x01;		// 8086/88 (MCS-80/85) mode
static const uint8_t ICW4_AUTO       = 0x02;		// Auto (normal) EOI
static const uint8_t ICW4_BUF_SLAVE  = 0x08;		// Buffered mode/slave
static const uint8_t ICW4_BUF_MASTER = 0x0C;		// Buffered mode/master
static const uint8_t ICW4_SFNM       = 0x10;		// Special fully nested (not)

static const int IDT_SIZE = 256;


static const uint8_t IRQ_0_VECTOR_START = 0x20;

namespace kernel
{

void initialize_PIC()
{
	// ICW1 - Start initialization in cascade mode
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	// ICW2 - Interrupt Vector offset
	outb(PIC1_DATA, IRQ_0_VECTOR_START); // Map IRQ 0..7 to 0x20
	io_wait();
	outb(PIC2_DATA, IRQ_0_VECTOR_START + 8); // Map IRQ 8..15 to 0x28
	io_wait();

	// ICW3 - PIC Cascading
	outb(PIC1_DATA, 4); // Tell Master PIC that there is a slave PIC at IRQ2 (0000_0100)
	io_wait();
	outb(PIC2_DATA, 2); // Tell Slave PIC its cascade identity (0000_0010)
	io_wait();

	// ICW4 - Environment information
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Disable all interrupts
	outb(PIC1_DATA , 0xff);
	outb(PIC2_DATA , 0xff);
}

static const uint8_t KERNEL_CODE_SEGMENT_OFFSET = 0x08;
static const uint8_t INTERRUPT_GATE = 0x8e;

struct IDT_entry {
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

extern "C" void load_idt(uint32_t *idt_ptr);

void register_callback(uint32_t callback_function_pointer, uint8_t irq)
{
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

} // namespace
