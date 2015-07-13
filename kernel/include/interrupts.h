#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

// Used for creating GDT segment descriptors in 64-bit integer form.
#include <stdio.h>
#include <stdint.h>

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

void initialize_PIC();
void register_callback(uint32_t callback_function_pointer, uint8_t irq);

} // namespace

#endif
