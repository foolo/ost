#include "Terminal.h"
#include <string.h>

namespace kernel {

Terminal::Terminal() :
m_row(0),
m_column(0),
m_color(make_color(COLOR_LIGHT_GREY, COLOR_BLACK)),
m_buffer((uint16_t*)VGA_COLOR_ADDRSPACE)
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		clear_line(y);
	}
}

Terminal::~Terminal() {
}

void Terminal::operator delete(void * /*p*/) // or delete(void *, size_t)
{
  // todo
}

void Terminal::writestring(const char* data) {
	size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; i++)
		putchar(data[i]);
}

void Terminal::putchar(char c) {
	if (c == '\n')
	{
		newline();
	}
	// place to add handling of more control characters
	else
	{
		putentryat(c, m_color, m_column, m_row);
		m_column++;
		if (m_column == VGA_WIDTH)
		{
			newline();
		}
	}
}

void Terminal::newline()
{
	m_column = 0;
	m_row++;
	if (m_row == VGA_HEIGHT) {
		m_row = VGA_HEIGHT - 1;
		scroll();
	}
}

void Terminal::scroll() {
	for(size_t row = 0; row < VGA_HEIGHT-1; row++)
	{
		void* pdst = (void*)(m_buffer + row * VGA_WIDTH);
		void* psrc = (void*)(m_buffer + (row + 1) * VGA_WIDTH);
		const size_t size_in_bytes = VGA_WIDTH * sizeof(*m_buffer);
		memcpy(pdst, psrc, size_in_bytes);
	}
	clear_line(VGA_HEIGHT - 1);
}

void Terminal::putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	m_buffer[index] = make_vgaentry(c, color);
}

void Terminal::setcolor(uint8_t color) {
	m_color = color;
}

void Terminal::clear_line(size_t line_number) {
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		const size_t index = line_number * VGA_WIDTH + x;
		m_buffer[index] = make_vgaentry(' ', m_color);
	}
}

uint16_t Terminal::make_vgaentry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

uint8_t Terminal::make_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

} /* namespace kernel */
