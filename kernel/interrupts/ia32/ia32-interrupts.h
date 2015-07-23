#ifndef IA32_INTERRUPTS_H_
#define IA32_INTERRUPTS_H_

#include "ia32/ia32-io.h"

static const uint8_t PIC1_COMMAND  = 0x20;   // Master PIC IO base address
static const uint8_t PIC1_DATA     = 0x21;
static const uint8_t PIC2_COMMAND  = 0xA0;   // Slave PIC IO base address
static const uint8_t PIC2_DATA     = 0xA1;

static const uint8_t IRQ_0_VECTOR_START = 0x20;

static const uint8_t KEYBOARD_IRQ = 1;

static const uint8_t SYSCALL_INTERRUPT_VECTOR = 0x80;


enum syscall_t
{
	SYSCALL_TEST
};

namespace kernel
{

inline void PIC_sendEOI(unsigned char irq)
{
	// End-of-interrupt command code
	const uint8_t PIC_EOI = 0x20;
	if (irq >= 8)
	{
		outb(PIC2_COMMAND, PIC_EOI);
	}
	outb(PIC1_COMMAND, PIC_EOI);
}

} // namespace

#endif
