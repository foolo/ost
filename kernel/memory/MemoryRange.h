#pragma once
#include <stdbool.h>

struct MemoryRange
{
	uint32_t m_start;
	uint32_t m_end;
	bool m_valid;
};
