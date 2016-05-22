#include <stdio.h>
#include "gdt.h"

namespace kernel
{

extern "C" void load_gdt(unsigned long *gdt_ptr);
extern "C" void reloadSegments(void);

uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t descriptor;

	// Create the high 32 bit segment
	descriptor = limit & 0x000F0000;         // set limit bits 19:16
	descriptor |= (flag << 8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
	descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
	descriptor |= base & 0xFF000000;         // set base bits 31:24

	// Shift by 32 to allow for low part of segment
	descriptor <<= 32;

	// Create the low 32 bit segment
	descriptor |= base << 16;                       // set base bits 15:0
	descriptor |= limit & 0x0000FFFF;               // set limit bits 15:0

	//printf("0x%.16llX\n", descriptor);
	return descriptor;
}

#define GDT_size 16
uint64_t GDT[GDT_size];

void load_gdt_main()
{
	// prepare address for GDT
	uint32_t idt_address = (uint32_t) GDT;
	uint32_t gdt_ptr[2];
	gdt_ptr[0] = (sizeof(uint64_t) * GDT_size) + ((idt_address & 0xffff) << 16);
	gdt_ptr[1] = idt_address >> 16;

	// fill the GDT descriptor
	load_gdt(gdt_ptr);
}

void initialize_GDT()
{
	GDT[0] = create_descriptor(0, 0, 0);
	GDT[1] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
	GDT[2] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
	GDT[3] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
	GDT[4] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
	load_gdt_main();
	reloadSegments();

	/*create_idt_entry((uint32_t)div0_handler_wrapper, DIVIDE_BY_ZERO_EXCEPTION_VECTOR);
	 create_idt_entry((uint32_t)keyboard_handler_wrapper, IRQ_0_VECTOR_START + KEYBOARD_IRQ);
	 enable_irq(KEYBOARD_IRQ);
	 create_idt_entry((uint32_t)syscall_handler_wrapper, SYSCALL_INTERRUPT_VECTOR);
	 load_idt_main();*/
}

} // namespace kernel
