#pragma once

#include <stdint.h>
#include <stdbool.h>

bool initialize_keyboard_controller();
bool is_keycode_down(uint8_t keycode);
bool is_keycode_down(uint8_t keycode);
void handle_keyboard_scancode();
bool read_scancode(uint8_t *scancode_out);
char keyboard_get_char();
