#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts.h"
#include "interrupts/ia32/ia32-interrupts.h" // todo just for test_syscall
#include "keyboard.h"
#include "multiboot_mmap.h"

namespace kernel
{

extern "C" void kernel_main(unsigned long magic, unsigned long addr)
{
	terminal_initialize();
//	multiboot_mmap(magic, (multiboot_info_t*) addr);
//	initialize_PIC();
//	initialize_IDT();
//	initialize_software_interrupts();

	printf("Hello, kernel World!\n");

	if (!keyboard::initialize_keyboard_controller())
	{
		printf("PS2 controller initialization failed\n");
	}
}

} /* namespace kernel */
