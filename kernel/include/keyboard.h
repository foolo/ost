#ifndef KEYBOARD_H_
#define KEYBOARD_H_

namespace kernel {
namespace keyboard {

void handle_keyboard_scancode();
bool read_scancode(uint8_t &scancode_out);

}} // namespace

#endif
