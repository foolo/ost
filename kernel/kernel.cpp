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

void test_syscall(int val1, int val2, int val3)
{
	int ret;
	asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(SYSCALL_TEST), "b"(val1), "c"(val2), "d"(val3)
        : "cc", "edi", "esi", "memory"
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
	test_syscall(123, 456, 789);
	test_syscall(111, 222, 333);
}

} /* namespace kernel */
