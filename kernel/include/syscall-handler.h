#ifndef SYSCALL_HANDLER_H_
#define SYSCALL_HANDLER_H_

#include <stdio.h>
#include <stdint.h>

namespace kernel {

int handle_syscall_write(int file, char *ptr, int len);
int handle_unknown_syscall(uint32_t syscall_id, uint32_t param1, uint32_t param2, uint32_t param3);

} // namespace

#endif
