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
#include "binfile.cpp"

extern addr_t kernel_start_address;
extern addr_t kernel_end_address;

extern "C" void jump_usermode(void);

namespace kernel
{

void load_user_process(uint32_t *kernelspace_page_directory) {

	uint32_t process_start_addr = 0x8048074;
	uint32_t process_entry_point = 0x8048080;

	uint32_t heap_size = 0x100000;

	uint32_t size = sizeof(binfile);
	uint32_t *dir = create_process_pgdir(process_start_addr, size + heap_size, kernelspace_page_directory);
	activate_page_directory((unsigned int*)dir);
	printf("Loading process, size: %lu\n", size);
	for (uint32_t i = 0; i < size; i++) {
		uint8_t* p = (uint8_t*)(process_start_addr + i);
		*p = binfile[i];
	}
	printf("First bytes: %lx\n", *((uint32_t*)process_entry_point));
}

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

	uint32_t *kernelspace_page_directory = create_kernel_pgdir();

	ide_initialize_parallel_ata();

	load_user_process(kernelspace_page_directory);
	jump_usermode();
}

} /* namespace kernel */
