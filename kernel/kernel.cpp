#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts.h"

#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

namespace kernel {

extern "C"
void kernel_main() {
	terminal_initialize();
	printf("Hello, kernel Worldz!\n");
	printf("test %c\n", 'x');
	printf("test %u\n", 7545);

	initialize_PIC();
	initialize_IDT();
}

} /* namespace kernel */
