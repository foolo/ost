#include "Terminal.h"

namespace kernel {

Terminal::Terminal() {
	// TODO Auto-generated constructor stub

}

Terminal::~Terminal() {
	// TODO Auto-generated destructor stub
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
		const size_t dst = row * VGA_WIDTH;
		const size_t src = (row + 1) * VGA_WIDTH;
		const size_t length = VGA_WIDTH;
		// todo use memcpy
		for(size_t i = 0; i < length; i++)
		{
			m_buffer[dst + i] = m_buffer[src + i];
		}
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

void Terminal::initialize() {
	m_row = 0;
	m_column = 0;
	m_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	m_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		clear_line(y);
	}
}

void Terminal::clear_line(size_t line_number) {
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		const size_t index = line_number * VGA_WIDTH + x;
		m_buffer[index] = make_vgaentry(' ', m_color);
	}
}

size_t Terminal::strlen(const char* str) {
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
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
