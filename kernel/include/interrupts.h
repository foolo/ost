#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <stdio.h>
#include <stdint.h>

namespace kernel {

void initialize_PIC();
void initialize_IDT();

} // namespace

#endif
