#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts.h"
#include "keyboard.h"

#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

namespace kernel {

void test_syscall()
{
    asm volatile
    (
        "int $0x80"
    );
}

extern "C"
void kernel_main() {
	terminal_initialize();
	printf("Hello, kernel Worldz!\n");
	printf("test %c\n", 'x');
	printf("test %u\n", 7545);

	initialize_PIC();
	initialize_IDT();
	if (!keyboard::initialize_keyboard_controller())
	{
		printf("PS2 controller initialization failed\n");
	}
	initialize_software_interrupts();
	test_syscall();
	test_syscall();
}

} /* namespace kernel */
