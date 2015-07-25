#include "syscall-handler.h"
#include "terminal.h"

namespace kernel
{

int handle_syscall_write(int /*file*/, char *ptr, int len)
{
	for (int i = 0; i < len; i++)
	{
		terminal_putchar(ptr[i]);
	}
	return 0;
}

int handle_unknown_syscall(uint32_t syscall_id, uint32_t param1, uint32_t param2, uint32_t param3)
{
	printf("Unknown syscall %lu with parameters %lu, %lu, %lu", syscall_id, param1, param2, param3);
	return 1;
}

} // namespace
