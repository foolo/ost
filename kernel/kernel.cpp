#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts.h"
#include "interrupts/ia32/ia32-interrupts.h" // todo just for test_syscall
#include "keyboard.h"

#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

namespace kernel {

extern "C"
void kernel_main() {
	initialize_PIC();
	initialize_IDT();
	initialize_software_interrupts();
	terminal_initialize();

	printf("Hello, kernel Worldz!\n");
	printf("test %c\n", 'x');
	printf("test %u\n", 7545);

	if (!keyboard::initialize_keyboard_controller())
	{
		printf("PS2 controller initialization failed\n");
	}
}

} /* namespace kernel */
