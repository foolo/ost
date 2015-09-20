#include <stdio.h>
#include "paging.h"
#include "memory/page_allocator.h"

extern "C" void loadPageDirectory(unsigned int*);
extern "C" void enablePaging();
extern addr_t kernel_end_address;

namespace kernel
{

uint32_t *initialize_table()
{
	uint32_t *page_table = (uint32_t*)allocate_frame();
	for(int i = 0; i < 1024; i++)
	{
		uint32_t physical_address = i * PAGE_SIZE;
		// Supervisor, write enabled, present
		uint32_t flags = PDFLAG_WRITABLE | PDFLAG_PRESENT;
		page_table[i] = physical_address | flags;
	}
	return page_table;
}

void set_up_page_tables(uint32_t *page_directory)
{
	int tables_to_fill = (kernel_end_address / PAGE_SIZE) + 1;
	printf("Initilaizing %i page tables for kernel\n", tables_to_fill);
	for (int i = 0; i < tables_to_fill; i++)
	{
		uint32_t *page_table = initialize_table();
		page_directory[i] = (uint32_t)page_table | PDFLAG_WRITABLE | PDFLAG_PRESENT;
	}
}

uint32_t *initialize_page_directory()
{
	uint32_t *page_directory = (uint32_t*)allocate_frame();
	for(int i = 0; i < 1024; i++)
	{
	    page_directory[i] = PDFLAG_WRITABLE;
	}
	return page_directory;
}

void set_up_paging()
{
	uint32_t *page_directory = initialize_page_directory();
	set_up_page_tables(page_directory);
	loadPageDirectory((unsigned int*)page_directory);
	enablePaging();
	printf("Paging was initialized\n");
}

} // namespace kernel
