#include <stddef.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "page_allocator.h"
#include "multiboot_mmap.h"
#include "memory/MemoryRange.h"

namespace kernel
{

uint32_t frame_map[TABLE_LENGTH];
unsigned table_start = 0;

// page aligned memory ranges [first_usable_page_address, last_usable_page_address]
// Example: A range of one megabyte starting at 0xC0000000 would end at 0xC00ff000
const int MEM_RANGES_CNT_MAX = 5;
int mem_ranges_counter = 0;
MemoryRange mem_ranges[MEM_RANGES_CNT_MAX];

void reset_page_allocator()
{
	for (unsigned i = 0; i < TABLE_LENGTH; i++)
	{
		frame_map[i] = 0x00000000;
	}
	for (int i = 0; i < MEM_RANGES_CNT_MAX; i++)
	{
		mem_ranges[i] = MemoryRange();
	}
	mem_ranges_counter = 0;
}

MemoryRange page_align_mem_range(const MemoryRange& mem_range)
{
	if (mem_range.GetEnd() < (PAGE_SIZE - 1))
	{
		return MemoryRange();
	}
	if (mem_range.GetStart() > 0xfffff000)
	{
		return MemoryRange();
	}

	addr_t start = round_up_to_page(mem_range.GetStart());
	addr_t end = round_down_to_page(mem_range.GetEnd() - (PAGE_SIZE - 1));

	if (start > end)
	{
		return MemoryRange();
	}
	return MemoryRange(start, end);
}

bool register_memory_range(const MemoryRange& mem_range)
{
	if (mem_ranges_counter >= 5)
	{
		return false;
	}
	MemoryRange range(page_align_mem_range(mem_range));
	if (range.IsValid())
	{
		mem_ranges[mem_ranges_counter++] = range;
	}
	return true;
}

void init_map(void* kernel_end_address)
{
	unsigned first_frame_after_kernel = (intptr_t)(kernel_end_address) / PAGE_SIZE;
	table_start = first_frame_after_kernel / 32 + 1;
	for (unsigned i = table_start; i < TABLE_LENGTH; i++)
	{
		frame_map[i] = 0xffffffff;
	}
	for(int i = 0; i < MEM_RANGES_CNT_MAX; i++)
	{
		printf("range: %lx .. %lx\n", (long unsigned)mem_ranges[i].GetStart(), (long unsigned)mem_ranges[i].GetEnd());
	}
}

inline pageframe_t table_index_to_address(unsigned i)
{
	// Get index of lowest 1-bit in map
	uint32_t map = frame_map[i];
	int lowest_set_bit = __builtin_ctz(map);
	// Translate iterator plus bit index to frame number
	intptr_t frame_number = i * 32 + lowest_set_bit;
	// Mark the frame as allocated by clearing the corresponding bit
	frame_map[i] &= ~(1 << lowest_set_bit);
	// Shift up the frame number to a physical address
	return (pageframe_t)(frame_number * PAGE_SIZE);
}

unsigned address_to_table_index(addr_t addr)
{
	unsigned page_index = addr / PAGE_SIZE;
	unsigned table_index = page_index / 32;
	return table_index;
}

unsigned address_to_bit_index(addr_t addr)
{
	unsigned bit_index = (addr / PAGE_SIZE) & 0x0000001f;
	return bit_index;
}

addr_t jump_to_next_map(addr_t addr)
{
	return (addr | (PAGE_SIZE * 32 - 1)) + 1;
}

pageframe_t allocate_frame()
{
	for (int i = 0; i < mem_ranges_counter; i++)
	{
		addr_t first = mem_ranges[i].GetStart();
		addr_t last = mem_ranges[i].GetEnd();

		addr_t addr = first;
		while (addr <= last)
		{
			unsigned table_index = address_to_table_index(addr);
			uint32_t &map = frame_map[table_index];
			if (map == 0x00000000)
			{
				addr = jump_to_next_map(addr);
			}
			else
			{
				unsigned bit_index = address_to_bit_index(addr);
				bool isfree = (map & (1 << bit_index)) != 0;
				if (isfree)
				{
					map &= ~(1 << bit_index);
					return (pageframe_t)addr;
				}
				addr += PAGE_SIZE;
			}
		}
	}
	return 0;
}

inline addr_t frame_address_to_frame_number(addr_t pf)
{
	// pf / PAGE_SIZE
	return pf >> 12;
}

void free_frame(addr_t pf)
{
	int frame_number = frame_address_to_frame_number(pf);
	int map_index = frame_number / 32;
	int bit_index = frame_number % 32;
	frame_map[map_index] |= (1 << bit_index);
}

} // namespace kernel
