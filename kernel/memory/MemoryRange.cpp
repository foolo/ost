#include <stdio.h>
#include "multiboot.h"
#include "multiboot_mmap.h"
#include <stdint.h>

namespace kernel
{

const int g_MEM_RANGES_COUNT = 5;
MemoryRange g_mem_ranges[g_MEM_RANGES_COUNT];

bool MemoryRange::IsValid() const
{
	return m_valid;
}

uint32_t MemoryRange::GetStart() const
{
	return m_start;
}

uint32_t MemoryRange::GetEnd() const
{
	return m_end;
}

} // namespace kernel
