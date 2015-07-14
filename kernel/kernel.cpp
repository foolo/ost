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


static const uint8_t KEYBOARD_DATA_PORT = 0x60;
static const uint8_t KEYBOARD_STATUS_PORT = 0x64;

extern "C" void keyboard_handler_main(void)
{
	kernel::PIC_sendEOI(1);

	uint8_t status = kernel::inb(KEYBOARD_STATUS_PORT);
	printf("status %u\n", status);
	if (status & 0x01)
	{
		uint8_t keycode = kernel::inb(KEYBOARD_DATA_PORT);
		printf("keycode %u\n", keycode);
	}
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
