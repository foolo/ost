#pragma once

#include <stddef.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory/types.h"

// number of pages = address space size / page size = 4GiB / 4KiB = 1Mi
// table length = number of pages / table width = 1Mi / 32 = 32768
#define TABLE_LENGTH 32768
#define PAGE_SIZE 0x1000

struct MemoryRange;

typedef void* pageframe_t;

void reset_page_allocator();
struct MemoryRange page_align_mem_range(const struct MemoryRange *mem_range);
bool register_memory_range(const struct MemoryRange *mem_range);
void init_map();
void allocate_kernel(void* kernel_start, void* kernel_end);
unsigned address_to_table_index(addr_t addr);
unsigned address_to_bit_index(addr_t addr);
addr_t jump_to_next_map(addr_t addr);
pageframe_t allocate_frame();
void print_map();
