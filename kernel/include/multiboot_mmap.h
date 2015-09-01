#ifndef GRUB_MMAP_H_
#define GRUB_MMAP_H_

#include "multiboot.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

namespace kernel
{

struct MemoryRange
{
public:
	uint32_t m_start;
	uint32_t m_end;
	MemoryRange()
	{
		m_start = 0;
		m_end = 0;
	}
	MemoryRange(uint32_t start, uint32_t end)
	{
		m_start = start;
		m_end = end;
	}
};

extern const int g_MEM_RANGES_COUNT;
extern MemoryRange g_mem_ranges[];

void multiboot_mmap(unsigned long magic, multiboot_info_t *mbi);

} // namespace kernel

#endif // GRUB_MMAP_H_
