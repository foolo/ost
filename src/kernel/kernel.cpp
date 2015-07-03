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

extern "C"
void kernel_main() {
	Terminal terminal;
	terminal.writestring("Hello, kernel World!\n");
}

} /* namespace kernel */
