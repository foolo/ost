#include <stdio.h>
#include "paging.h"
#include "memory/page_allocator.h"

extern "C" void enablePaging();
extern addr_t kernel_end_address;

namespace kernel
{

const int TABLE_SIZE = 1024;
uint32_t kernelspace_end_address;

uint32_t get_kernel_size_in_tables() {
	return ((uint32_t)(&kernel_end_address) / PAGE_SIZE / TABLE_SIZE) + 1;
}

uint32_t *initialize_table(uint32_t flags)
{
	uint32_t *page_table = (uint32_t*)allocate_frame();
	for(int i = 0; i < TABLE_SIZE; i++)
	{
		uint32_t physical_address = i * PAGE_SIZE;
		// Supervisor, write enabled, present
		page_table[i] = physical_address | flags;
	}
	return page_table;
}

void set_up_kernel_page_tables(uint32_t *pagedir)
{
	int tables_to_fill = get_kernel_size_in_tables();
	printf("Initilaizing %i pages for kernel\n", tables_to_fill);
	for (int i = 0; i < tables_to_fill; i++)
	{
		uint32_t *page_table = initialize_table(PDFLAG_WRITABLE | PDFLAG_PRESENT);
		pagedir[i] = (uint32_t)page_table | PDFLAG_WRITABLE | PDFLAG_PRESENT;
	}
}

void map_kernelspace_in_process(uint32_t *process_pgdir, uint32_t *kernel_pgdir)
{
	int tables_to_fill = get_kernel_size_in_tables();
	printf("Mapping %i kernel page tables in process\n", tables_to_fill);
	for (int i = 0; i < tables_to_fill; i++)
	{
		process_pgdir[i] = kernel_pgdir[i] | PDFLAG_USER_PREVILEGES;
	}
}

void set_up_userspace_page_tables(uint32_t *userspace_pagedir, uint32_t virtual_start_address, uint32_t size)
{
	uint32_t tables_to_fill = ((size-1) / PAGE_SIZE / TABLE_SIZE) + 1;
	uint32_t start_table = virtual_start_address >> 22;
	printf("Initilaizing %lu page tables for process, start: %lx, end: %lx\n", tables_to_fill, start_table << 22, (start_table+tables_to_fill) << 22);
	for (uint32_t table = start_table; table < start_table + tables_to_fill; table++)
	{
		if (userspace_pagedir[table] & PDFLAG_PRESENT) {
			continue;
		}
		uint32_t *page_table = initialize_table(PDFLAG_WRITABLE | PDFLAG_PRESENT | PDFLAG_USER_PREVILEGES);
		userspace_pagedir[table] = (uint32_t)page_table | PDFLAG_WRITABLE | PDFLAG_PRESENT | PDFLAG_USER_PREVILEGES;
	}
}

uint32_t *initialize_page_directory(uint32_t flags)
{
	uint32_t *page_directory = (uint32_t*)allocate_frame();
	for(int i = 0; i < 1024; i++)
	{
	    page_directory[i] = flags;
	}
	return page_directory;
}

uint32_t *create_kernel_pgdir()
{
	uint32_t *pgdir = initialize_page_directory(PDFLAG_WRITABLE);
	set_up_kernel_page_tables(pgdir);
	activate_page_directory((unsigned int*)pgdir);
	enablePaging();
	printf("Paging was initialized\n");
	return pgdir;
}

} // namespace kernel
