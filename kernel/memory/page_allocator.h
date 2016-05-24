#pragma once

#include <stddef.h>
#include <inttypes.h>
#include <stdint.h>
#include "memory/types.h"

// number of pages = address space size / page size = 4GiB / 4KiB = 1Mi
// table length = number of pages / table width = 1Mi / 32 = 32768
#define TABLE_LENGTH 32768
#define PAGE_SIZE 0x1000

namespace kernel
{

struct MemoryRange;

typedef void* pageframe_t;

inline bool is_page_aligned(addr_t addr)
{
	return (addr & 0x0fff) == 0;
}

// Used for start of range
inline addr_t round_up_to_page(addr_t addr)
{
	if (!is_page_aligned(addr)) {
		return (addr & 0xfffff000) + 0x1000; // next whole page
	}
	return addr;
}

// Used for end of range
inline addr_t round_down_to_page(addr_t addr)
{
	return (addr & 0xfffff000);
}
void reset_page_allocator();
MemoryRange page_align_mem_range(const MemoryRange& mem_range);
bool register_memory_range(const MemoryRange& mem_range);
void init_map();
void allocate_kernel(void* kernel_start, void* kernel_end);
inline pageframe_t table_index_to_address(unsigned i);
unsigned address_to_table_index(addr_t addr);
unsigned address_to_bit_index(addr_t addr);
addr_t jump_to_next_map(addr_t addr);
pageframe_t allocate_frame();
inline intptr_t frame_address_to_frame_number(pageframe_t pf);
void free_frame(pageframe_t pf);
void print_map();

} // namespace kernel