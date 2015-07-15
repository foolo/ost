#include "interrupts.h"
#include "ia32-interrupts.h"

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

namespace kernel
{

void initialize_PIC()
{
	// ICW1 - Start initialization in cascade mode
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

	// ICW2 - Interrupt Vector offset
	outb(PIC1_DATA, IRQ_0_VECTOR_START); // Map IRQ 0..7 to 0x20
	outb(PIC2_DATA, IRQ_0_VECTOR_START + 8); // Map IRQ 8..15 to 0x28

	// ICW3 - PIC Cascading
	outb(PIC1_DATA, 4); // Tell Master PIC that there is a slave PIC at IRQ2 (0000_0100)
	outb(PIC2_DATA, 2); // Tell Slave PIC its cascade identity (0000_0010)

	// ICW4 - Environment information
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);

	// Disable all interrupts
	outb(PIC1_DATA , 0xff);
	outb(PIC2_DATA , 0xff);
}

} // namespace
