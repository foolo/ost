#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts/interrupts.h"
#include "interrupts/ia32/ia32-interrupts.h" // todo just for test_syscall
#include "keyboard/keyboard.h"
#include "memory/multiboot_mmap.h"
#include "memory/page_allocator.h"
#include "memory/ia32/gdt.h"
#include "memory/ia32/paging.h"
#include "storage/ata/ia32/ide_controller.h"
#include <stdlib.h>


extern addr_t kernel_start_address;
extern addr_t kernel_end_address;

int main(int argc, char **argv);


namespace kernel
{

extern "C" void kernel_main(unsigned long magic, unsigned long addr)
{
	terminal_initialize();
	initialize_PIC();
	initialize_GDT();
	initialize_IDT();

	printf("Hello, kernel World!\n");
	printf("kernel start: %p\n", &kernel_start_address);
	printf("kernel end:   %p\n", &kernel_end_address);

	reset_page_allocator();
	multiboot_mmap(magic, (multiboot_info_t*) addr);
	init_map();
	allocate_kernel(&kernel_start_address, &kernel_end_address);

	print_map();

	if (!keyboard::initialize_keyboard_controller())
	{
		printf("PS2 controller initialization failed\n");
	}

	set_up_paging();

	ide_initialize_parallel_ata();

	char pyname[] = "python";
	char *pyargv[1];
	pyargv[0] = pyname;
	main(1, pyargv);
}

} /* namespace kernel */
