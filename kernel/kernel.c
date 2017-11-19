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
#include "process/elf_loader.h"
#include "storage/ata/ia32/ide_controller.h"
#include "util/md5/md5digest.h"

extern addr_t kernel_start_address;
extern addr_t kernel_end_address;
extern addr_t _initramfs_start;
extern addr_t _initramfs_end;

extern "C" void jump_usermode(addr_t entry_point);

uint8_t *binfile;
uint32_t binfile_size;

namespace kernel
{

void load_init_process(uint32_t *kernelspace_page_directory) {

	binfile = (uint8_t*)&_initramfs_start;
	binfile_size = (uint32_t)&_initramfs_end - (uint32_t)&_initramfs_start;

	uint32_t *pgdir = initialize_page_directory(PDFLAG_WRITABLE | PDFLAG_USER_PREVILEGES);
	map_kernelspace_in_process(pgdir, kernelspace_page_directory);
	// allocate 1MB stack space
	set_up_userspace_page_tables(pgdir, 0xFFF00000, 0x100000);
	// todo allocate heap
	//uint32_t heap_size = 0x100000;
	activate_page_directory((unsigned int*)pgdir);
	elf32_file_header fh;
	if (load_elf(-1, pgdir, fh)) {
		jump_usermode(fh.e_entry);
	}
	else {
		printf("load elf failed\n");
	}
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

	load_init_process(kernelspace_page_directory);
}

} /* namespace kernel */
