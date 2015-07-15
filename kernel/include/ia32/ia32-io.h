#ifndef IA32_IO_H_
#define IA32_IO_H_

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

} // namespace

#endif
