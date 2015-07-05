#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stddef.h>
#include <stdint.h>


namespace kernel {

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

//TODO create architecture independent interface ITerminal and make this class "i386Terminal"

class Terminal {
private:
	static const size_t VGA_WIDTH = 80;
	static const size_t VGA_HEIGHT = 25;
	static const uint32_t VGA_COLOR_ADDRSPACE = 0xB8000;

	// TODO should be global to prevent two instances of Terminal writing on different screen positions?
	size_t m_row;
	size_t m_column;
	uint8_t m_color;
	uint16_t* m_buffer;

public:
	Terminal();
	virtual ~Terminal();

	void initialize();
	void writestring(const char* data);
	void putchar(char c);

private:
	void newline();
	void scroll();
	void putentryat(char c, uint8_t color, size_t x, size_t y);
	void setcolor(uint8_t color);
	void clear_line(size_t line_number);
	uint16_t make_vgaentry(char c, uint8_t color);
	uint8_t make_color(enum vga_color fg, enum vga_color bg);
};

} /* namespace kernel */
#endif /* TERMINAL_H_ */
