#include <stdio.h>
#include <stdint.h>
#include "keyboard.h"
#include "layout.h"

static bool key_down_map[256];

bool is_keycode_down(uint8_t keycode) {
	return key_down_map[keycode];
}

void handle_keyboard_scancode()
{
	static bool up_state;
	static bool ext_state;
	uint8_t scancode;
	if (read_scancode(&scancode))
	{
		switch(scancode) {
		case 0xf0:
			up_state = true;
			break;
		case 0xe0:
			ext_state = true;
			break;
		default: {
			uint8_t keycode = (ext_state) ? (scancode + 0x80) : (scancode);
			ext_state = false;
			if (up_state) {
				key_down_map[keycode] = false;
				up_state = false;
			}
			else {
				key_down_map[keycode] = true;
				layout_handle_keycode(keycode);
			}
		}
		}
		//printf("scancode %x\n", scancode);
	}
}
