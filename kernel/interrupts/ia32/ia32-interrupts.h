static const uint8_t IRQ_0_VECTOR_START = 0x20;

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
