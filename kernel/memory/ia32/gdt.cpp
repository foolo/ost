#include <stdio.h>
#include <string.h>
#include "gdt.h"


extern "C" void sample_user_function() {
	int i = 0;
	while(1) {
		printf("user space %i\n", i++);
	}
}

namespace kernel
{

extern "C" void load_gdt(unsigned long *gdt_ptr);
extern "C" void reloadSegments(void);
extern "C" void activate_tss(uint16_t tss_gdt_index);
extern "C" uint32_t get_esp(void);


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

tss_entry_t tss_entry;

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

	uint32_t tss_base = (uint32_t) &tss_entry;
	uint32_t tss_limit = sizeof(tss_entry);
	GDT[5] = create_descriptor(tss_base,  tss_limit, (GDT_TSS)); //0x28

	memset((void*)&tss_entry, 0, (unsigned)sizeof(tss_entry));
	tss_entry.ss0 = 0x10;
	tss_entry.esp0 = get_esp(); // TODO kernel stack address

	load_gdt_main();
	reloadSegments();

	activate_tss(0x2B);  // 5th position in GDT, times 8 = 0x28. Plus 3 for RPL 3
}

} // namespace kernel
