#include <stdio.h>
#include <stdint.h>
#include "keyboard.h"

namespace kernel {
namespace keyboard {

void handle_keyboard_scancode()
{
	uint8_t scancode;
	if (read_scancode(scancode))
	{
		printf("scancode %x\n", scancode);
	}
}

}} // namespace