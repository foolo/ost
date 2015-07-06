#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"

#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

namespace kernel {

extern "C"
void kernel_main() {
	terminal_initialize();
	printf("Hello, kernel World!\n");
	printf("test %c\n", 'x');
}

} /* namespace kernel */
