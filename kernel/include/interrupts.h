#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <stdio.h>
#include <stdint.h>

static const uint8_t PIC1_COMMAND  = 0x20;   // Master PIC IO base address
static const uint8_t PIC1_DATA     = 0x21;
static const uint8_t PIC2_COMMAND  = 0xA0;   // Slave PIC IO base address
static const uint8_t PIC2_DATA     = 0xA1;

namespace kernel {

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "d"(port) );
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "d"(port) );
    return ret;
}

static inline void io_wait()
{
    // Port 0x80 is used for 'checkpoints' during POST.
    // The Linux kernel seems to think it is free for use
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

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

void initialize_PIC();
void register_callback(uint32_t callback_function_pointer, uint8_t irq);

} // namespace

#endif
