#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts/interrupts.h"
#include "keyboard/keyboard.h"
#include "memory/multiboot_mmap.h"
#include "memory/page_allocator.h"
#include "memory/ia32/gdt.h"
#include "memory/ia32/paging.h"
#include "process/elf_loader.h"
#include "process/process.h"
#include "storage/ata/ia32/ide_controller.h"
#include "fs/fs.h"
#include "util/cpio_loader.h"
#include "util/assert.h"

extern addr_t kernel_start_address;
extern addr_t kernel_end_address;
extern addr_t _initramfs_start;
extern addr_t _initramfs_end;

void jump_usermode(addr_t entry_point);

void load_init_process(uint32_t *kernelspace_page_directory) {

	unsigned pid = create_new_process_id();
	struct process_info *pr = get_process_info(pid);
	pr->pgdir = initialize_page_directory(PDFLAG_WRITABLE | PDFLAG_USER_PREVILEGES);
	map_kernelspace_in_process(pr->pgdir, kernelspace_page_directory);
	// allocate 1MB stack space
	set_up_userspace_page_tables(pr->pgdir, 0xFFF00000, 0x100000);
	activate_page_directory((unsigned int*)pr->pgdir);
	struct elf32_file_header fh;
	struct file f;
	struct file root_dir = dir_open_root();
	if (!file_open("init", &f, &root_dir)) {
		printf("file open failed\n");
		return;
	}
	if (load_elf(&f, pr->pgdir, &fh, pr)) {
		set_current_process(pid);
		jump_usermode(fh.e_entry);
	}
	else {
		// todo deallocate pages
		printf("load elf failed\n");
	}
}

void load_initramfs() {
	create_root_inode();
	void *binfile = (void*)&_initramfs_start;
	uint32_t binfile_size = (uint32_t)&_initramfs_end - (uint32_t)&_initramfs_start;
	if (!load_cpio(binfile, binfile_size)) {
		halt("load_cpio failed");
	}
}


void kernel_main(unsigned long magic, unsigned long addr)
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

	if (!initialize_keyboard_controller())
	{
		printf("PS2 controller initialization failed\n");
	}

	uint32_t *kernelspace_page_directory = create_kernel_pgdir();

	ide_initialize_parallel_ata();

	load_initramfs();

	load_init_process(kernelspace_page_directory);
}
