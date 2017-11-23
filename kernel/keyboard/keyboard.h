#pragma once

#include <stdbool.h>

bool initialize_keyboard_controller();
void handle_keyboard_scancode();
bool read_scancode(uint8_t *scancode_out);
