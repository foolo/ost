#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <stdio.h>
#include <stdint.h>

namespace kernel {

void initialize_PIC();
void register_callback(uint32_t callback_function_pointer, uint8_t irq);

} // namespace

#endif
