#include <stdio.h>
#include <stdint.h>
#include "keyboard.h"
#include "ia32/ia32-io.h"

namespace kernel {
namespace keyboard {

bool read_scancode(uint8_t &scancode_out)
{
	static const uint8_t KEYBOARD_DATA_PORT = 0x60;
	static const uint8_t KEYBOARD_STATUS_PORT = 0x64;
	uint8_t status = kernel::inb(KEYBOARD_STATUS_PORT);
	if (status & 0x01)
	{
		scancode_out = kernel::inb(KEYBOARD_DATA_PORT);
		return true;
	}
	return false;
}

}} // namespace
