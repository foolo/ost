#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "terminal.h"
#include "interrupts.h"

#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif

extern "C" void keyboard_handler(void);
extern "C" void load_idt(unsigned long *idt_ptr);

extern "C" void keyboard_handler_main(void)
{
	printf("KB INT\n");
}

namespace kernel {


extern "C"
void kernel_main() {
	terminal_initialize();
	printf("Hello, kernel Worldz!\n");
	printf("test %c\n", 'x');
	printf("test %u\n", 7545);

	initialize_PIC();
	register_callback((uint32_t)keyboard_handler, 1);

}

} /* namespace kernel */
