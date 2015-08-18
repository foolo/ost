#include <stddef.h>
#include <inttypes.h>
#include <stdint.h>
#include "page_allocator.h"

uint32_t frame_map[TABLE_LENGTH];
unsigned table_start = 0;
unsigned static_i = 0;

void init_map(void* kernel_end_address)
{
	unsigned first_frame_after_kernel = (intptr_t)(kernel_end_address) / PAGE_SIZE;
	table_start = first_frame_after_kernel / 32 + 1;
	for (unsigned i = table_start; i < TABLE_LENGTH; i++)
	{
		frame_map[i] = 0xffffffff;
	}
	static_i = table_start;
}

inline pageframe_t table_index_to_address(unsigned i)
{
	// Get index of lowest 1-bit in map
	int lowest_set_bit = __builtin_ctz(frame_map[i]);
	// Translate iterator plus bit index to frame number
	intptr_t frame_number = i * 32 + lowest_set_bit;
	// Mark the frame as allocated by clearing the corresponding bit
	frame_map[i] &= ~(1 << lowest_set_bit);
	// Shift up the frame number to a physical address
	return (pageframe_t)(frame_number * PAGE_SIZE);
}


pageframe_t allocate_frame()
{
	unsigned count_start = static_i;
	while (static_i < TABLE_LENGTH)
	{
		if (frame_map != 0x00000000)
		{
			return table_index_to_address(static_i);
		}
		static_i++;
	}
	static_i = table_start;
	while (static_i < count_start)
	{
		if (frame_map != 0x00000000)
		{
			return table_index_to_address(static_i);
		}
		static_i++;
	}
	return NULL;
}

inline intptr_t frame_address_to_frame_number(pageframe_t pf)
{
	// pf / PAGE_SIZE
	return (intptr_t)pf >> 12;
}

void free_frame(pageframe_t pf)
{
	int frame_number = frame_address_to_frame_number(pf);
	int map_index = frame_number / 32;
	int bit_index = frame_number % 32;
	frame_map[map_index] |= (1 << bit_index);
}
