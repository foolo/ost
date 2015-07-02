#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "terminal/i386/Terminal.h"
 
#if !defined(__i386__)
#error "ix86-elf compiler required"
#endif


#if defined(__cplusplus)
extern "C"
#endif
// todo move to namespace kernel
void kernel_main() {
	kernel::Terminal terminal;
	terminal.writestring("Hello, kernel World!\n");
}
