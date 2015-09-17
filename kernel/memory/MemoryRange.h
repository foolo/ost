#pragma once

namespace kernel {

class MemoryRange
{
	uint32_t m_start;
	uint32_t m_end;
	bool m_valid;

public:
	MemoryRange() :
			m_start(0), m_end(0), m_valid(false)
	{
	}
	MemoryRange(uint32_t start, uint32_t end) :
			m_start(start), m_end(end), m_valid(true)
	{
	}

	bool IsValid() const;
	uint32_t GetStart() const;
	uint32_t GetEnd() const;
};

} // namespace kernel
