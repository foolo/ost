#include <stdint.h>
#include <string.h>
#include "terminal.h"

size_t m_row;
size_t m_column;
uint8_t m_color;
uint16_t* m_buffer;

enum vga_color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static const uint32_t VGA_COLOR_ADDRSPACE = 0xB8000;


uint16_t make_vgaentry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

void clear_line(size_t line_number) {
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		const size_t index = line_number * VGA_WIDTH + x;
		m_buffer[index] = make_vgaentry(' ', m_color);
	}
}

void scroll() {
	for(size_t row = 0; row < VGA_HEIGHT-1; row++)
	{
		void* pdst = (void*)(m_buffer + row * VGA_WIDTH);
		void* psrc = (void*)(m_buffer + (row + 1) * VGA_WIDTH);
		const size_t size_in_bytes = VGA_WIDTH * sizeof(*m_buffer);
		memcpy(pdst, psrc, size_in_bytes);
	}
	clear_line(VGA_HEIGHT - 1);
}

void newline()
{
	m_column = 0;
	m_row++;
	if (m_row == VGA_HEIGHT) {
		m_row = VGA_HEIGHT - 1;
		scroll();
	}
}

void putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	m_buffer[index] = make_vgaentry(c, color);
}

void setcolor(uint8_t color) {
	m_color = color;
}

void terminal_initialize() {
	m_row = 0;
	m_column = 0;
	m_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	m_buffer = (uint16_t*)VGA_COLOR_ADDRSPACE;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		clear_line(y);
	}
}

void terminal_putchar(char c) {
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
