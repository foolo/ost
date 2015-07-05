#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "terminal/i386/Terminal.h"
 
#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

namespace kernel {

void print_uint(unsigned int i, Terminal& terminal)
{
	unsigned upper = i / 10;
	unsigned lower = i % 10;
	if(upper > 0)
	{
		print_uint(upper, terminal);
	}
	char buf[2];
	buf[1] = 0;
	buf[0] = lower + '0';
	terminal.writestring(buf);
}

extern "C"
void kernel_main() {
	Terminal terminal;
	terminal.writestring("Hello, kernel World!\n");
}

} /* namespace kernel */
