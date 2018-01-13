#include <stdbool.h>
#include <stdio.h>
#include "keyboard.h"
#include "layout.h"
#include "input_queue.h"

const int keycode_to_symbol_map_en[256] = {
		[0x01] = KEY_F9,
		[0x03] = KEY_F5,
		[0x04] = KEY_F3,
		[0x05] = KEY_F1,
		[0x06] = KEY_F2,
		[0x07] = KEY_F12,
		[0x09] = KEY_F10,
		[0x0A] = KEY_F8,
		[0x0B] = KEY_F6,
		[0x0C] = KEY_F4,
		[0x0D] = '\t',
		[0x0E] = '`',
		[0x11] = KEY_L_ALT,
		[0x12] = KEY_L_SHIFT,
		[0x14] = KEY_L_CTRL,
		[0x15] = 'Q',
		[0x16] = '1',
		[0x1A] = 'Z',
		[0x1B] = 'S',
		[0x1C] = 'A',
		[0x1D] = 'W',
		[0x1E] = '2',
		[0x21] = 'C',
		[0x22] = 'X',
		[0x23] = 'D',
		[0x24] = 'E',
		[0x25] = '4',
		[0x26] = '3',
		[0x29] = ' ',
		[0x2A] = 'V',
		[0x2B] = 'F',
		[0x2C] = 'T',
		[0x2D] = 'R',
		[0x2E] = '5',
		[0x31] = 'N',
		[0x32] = 'B',
		[0x33] = 'H',
		[0x34] = 'G',
		[0x35] = 'Y',
		[0x36] = '6',
		[0x3A] = 'M',
		[0x3B] = 'J',
		[0x3C] = 'U',
		[0x3D] = '7',
		[0x3E] = '8',
		[0x41] = ',',
		[0x42] = 'K',
		[0x43] = 'I',
		[0x44] = 'O',
		[0x45] = '0',
		[0x46] = '9',
		[0x49] = '0',
		[0x4A] = '/',
		[0x4B] = 'L',
		[0x4C] = ';',
		[0x4D] = 'P',
		[0x4E] = '-',
		[0x52] = '\'',
		[0x54] = '[',
		[0x55] = '=',
		[0x58] = KEY_CAPS_LOCK,
		[0x59] = KEY_R_SHIFT,
		[0x5A] = KEY_ENTER,
		[0x5B] = ']',
		[0x5D] = '\\',
		[0x66] = '\b',
		[0x69] = KEY_KP_1,
		[0x6B] = KEY_KP_4,
		[0x6C] = KEY_KP_7,
		[0x70] = KEY_KP_0,
		[0x71] = KEY_KP_DELIM,
		[0x72] = KEY_KP_2,
		[0x73] = KEY_KP_5,
		[0x74] = KEY_KP_6,
		[0x75] = KEY_KP_8,
		[0x76] = '\x1b',
		[0x77] = KEY_NUM_LOCK,
		[0x78] = KEY_F11,
		[0x79] = KEY_KP_PLUS,
		[0x7A] = KEY_KP_3,
		[0x7B] = KEY_KP_MINUS,
		[0x7C] = KEY_KP_MULTIPLY,
		[0x7D] = KEY_KP_9,
		[0x7E] = KEY_SCROLL_LOCK,
		[0x83] = KEY_F7,
		[0x11 + 0x80] = KEY_R_ALT,
		[0x14 + 0x80] = KEY_R_CTRL,
		[0x1F + 0x80] = KEY_L_GUI,
		[0x27 + 0x80] = KEY_R_GUI,
		[0x2F + 0x80] = KEY_APPS,
		[0x4A + 0x80] = KEY_KP_DIVIDE,
		[0x5A + 0x80] = KEY_KP_ENTER,
		[0x69 + 0x80] = KEY_END,
		[0x6B + 0x80] = KEY_ARROW_L,
		[0x6C + 0x80] = KEY_HOME,
		[0x70 + 0x80] = KEY_INSERT,
		[0x71 + 0x80] = KEY_DELETE,
		[0x72 + 0x80] = KEY_ARROW_D,
		[0x74 + 0x80] = KEY_ARROW_R,
		[0x75 + 0x80] = KEY_ARROW_U,
		[0x7A + 0x80] = KEY_PAGE_DOWN,
		[0x7D + 0x80] = KEY_PAGE_UP,
};

uint32_t keycode_to_unicode_en(struct key_event *event) {
	bool shift = (event->modifiers & MODIFIER_LEFT_SHIFT) || (event->modifiers & MODIFIER_RIGHT_SHIFT);
	bool ctrl = (event->modifiers & MODIFIER_LEFT_CTRL) || (event->modifiers & MODIFIER_RIGHT_CTRL);
	bool alt = (event->modifiers & MODIFIER_LEFT_ALT);
	bool altgr = (event->modifiers & MODIFIER_RIGHT_ALT);

	if (!ctrl && !alt && !altgr) {
		if (event->symbol >= 'A' && event->symbol <= 'Z') {
			if (shift) {
				return event->symbol + 0x20;
			}
			else {
				return event->symbol;
			}
		}
		if (event->symbol == KEY_ENTER) {
			return '\n';
		}
	}
	return -1;
}

void push_utf8(uint32_t unicode) {
	printf("push_utf8 %x\n", unicode);
	uint8_t buf[4];
	if (unicode < 0x80) {
		/*  0******* */
		buf[0] = unicode;
		input_queue_push(buf, 1);
	}
	else if (unicode < 0x800) {
		/* 110***** 10****** */
		buf[0] = 0xc0 | (unicode >> 6);
		buf[1] = 0x80 | (unicode & 0x3f);
		input_queue_push(buf, 2);
	}
	else if (unicode < 0x10000) {
		if (unicode >= 0xD800 && unicode < 0xE000) {
			return;
		}
		if (unicode == 0xFFFF) {
			return;
		}
		/* 1110**** 10****** 10****** */
		buf[0] = 0xe0 | (unicode >> 12);
		buf[1] = 0x80 | ((unicode >> 6) & 0x3f);
		buf[2] = 0x80 | (unicode & 0x3f);
		input_queue_push(buf, 3);
	}
	else if (unicode < 0x110000) {
		/* 11110*** 10****** 10****** 10****** */
		buf[0] = 0xf0 | (unicode >> 18);
		buf[1] = 0x80 | ((unicode >> 12) & 0x3f);
		buf[2] = 0x80 | ((unicode >> 6) & 0x3f);
		buf[3] = 0x80 | (unicode & 0x3f);
		input_queue_push(buf, 4);
	}
}

void layout_handle_keycode_en(uint8_t keycode) {
	struct key_event event;
	event.modifiers = 0;
	event.modifiers |= is_keycode_down(0x77) ? MODIFIER_NUM_LOCK : 0;
	event.modifiers |= is_keycode_down(0x58) ? MODIFIER_CAPS_LOCK : 0;
	event.modifiers |= is_keycode_down(0x14) ? MODIFIER_LEFT_CTRL : 0;
	event.modifiers |= is_keycode_down(0x14 + 0x80) ? MODIFIER_RIGHT_CTRL : 0;
	event.modifiers |= is_keycode_down(0x12) ? MODIFIER_LEFT_SHIFT : 0;
	event.modifiers |= is_keycode_down(0x59) ? MODIFIER_RIGHT_SHIFT : 0;
	event.modifiers |= is_keycode_down(0x11) ? MODIFIER_LEFT_ALT : 0;
	event.modifiers |= is_keycode_down(0x11 + 0x80) ? MODIFIER_RIGHT_ALT : 0;
	event.symbol = keycode_to_symbol_map_en[keycode];
	event.state = is_keycode_down(keycode);
	event.unicode = keycode_to_unicode_en(&event);
	push_utf8(event.unicode);
}

void layout_handle_keycode(uint8_t keycode) {
	layout_handle_keycode_en(keycode);
}
