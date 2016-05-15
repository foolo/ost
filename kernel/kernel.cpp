#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts.h"

// Used for creating GDT segment descriptors in 64-bit integer form.

#include <stdio.h>
#include <stdint.h>

// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)

#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed

#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_CODE_EXRD

#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_DATA_RDWR

#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_CODE_EXRD

#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_DATA_RDWR

#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

extern "C" void keyboard_handler(void);
extern "C" void load_idt(unsigned long *idt_ptr);
extern "C" void div0_handler();
extern "C" void load_gdt(uint32_t address, uint16_t size);
extern "C" void reloadSegments(void);

static const uint8_t KEYBOARD_DATA_PORT = 0x60;
static const uint8_t KEYBOARD_STATUS_PORT = 0x64;

extern "C" void div0_handler_main(void) {
	printf("division by zero\n");
	while(1);
}

extern "C" void keyboard_handler_main(void)
{
	kernel::PIC_sendEOI(1);

	uint8_t status = kernel::inb(KEYBOARD_STATUS_PORT);
	printf("status %u\n", status);
	if (status & 0x01)
	{
		uint8_t keycode = kernel::inb(KEYBOARD_DATA_PORT);
		printf("keycode %u\n", keycode);
	}
}

extern "C" void read_idt(uint32_t *idt_ptr);
extern "C" void read_gdt(uint32_t *gdt_ptr);

namespace kernel {


void print_idt() {
	uint32_t idt_ptr[2];
	read_idt(idt_ptr);
	printf("idt1: %x\n", idt_ptr[1]);
	printf("idt0: %x\n", idt_ptr[0]);
}


void print_gdt() {
	uint32_t buf[2];
	read_gdt(buf);
	printf("gdt1: %x\n", buf[1]);
	printf("gdt0: %x\n", buf[0]);
}

void print_cr(){
	uint32_t cr0 = 0;
	uint32_t cr2 = 0;
	uint32_t cr3 = 0;
	uint32_t cr4 = 0;

	asm volatile ( "mov %%cr0, %0" : "=r"(cr0) );
	asm volatile ( "mov %%cr2, %0" : "=r"(cr2) );
	asm volatile ( "mov %%cr3, %0" : "=r"(cr3) );
	asm volatile ( "mov %%cr4, %0" : "=r"(cr4) );

	printf("cr0: %x\n", cr0);
	printf("cr2: %x\n", cr2);
	printf("cr3: %x\n", cr3);
	printf("cr4: %x\n", cr4);
}

void print_irq_mask() {
	const uint8_t PIC1_DATA     = 0x21;
	const uint8_t PIC2_DATA     = 0xA1;
	uint8_t mask1 = inb(PIC1_DATA);
	uint8_t mask2 = inb(PIC2_DATA);
	printf("mask1: %x\n", mask1);
	printf("mask2: %x\n", mask2);
}

uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor;

    // Create the high 32 bit segment
    descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    descriptor |=  base        & 0xFF000000;         // set base bits 31:24

    // Shift by 32 to allow for low part of segment
    descriptor <<= 32;

    // Create the low 32 bit segment
    descriptor |= base  << 16;                       // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0

    //printf("0x%.16llX\n", descriptor);
    return descriptor;
}

#define GDT_size 16
uint64_t GDT[GDT_size];

extern "C"
void kernel_main() {
	terminal_initialize();
	print_cr();

	unsigned char a1, a2;
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
	printf("a1: %x\n", a1);
	printf("a2: %x\n", a2);

	printf("Hello, kernel World!\n");

	print_idt();
	print_gdt();

	const uint8_t IRQ_0_VECTOR_START = 0x20;
	uint8_t keyboard_irq = 1;
	uint8_t kb_vector_index = IRQ_0_VECTOR_START + keyboard_irq;

	register_callback((uint32_t)keyboard_handler, kb_vector_index );
	register_callback((uint32_t)div0_handler, 0x0 );

	GDT[0] = create_descriptor(0, 0, 0);
	GDT[1] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
	GDT[2] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
	GDT[3] = create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
	GDT[4] = create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
	load_gdt((uint32_t)GDT, 16);
	//reloadSegments();

	load_idt_main();


	initialize_PIC();
	enable_irq(keyboard_irq);



	//int b = 0; int a = 5 / b; printf ("test a: %i\n", a);

	print_idt();

	print_irq_mask();
}

} /* namespace kernel */
