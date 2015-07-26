#include "syscall-handler.h"
#include "terminal.h"

namespace kernel
{

void handle_syscall_exit(int /*file*/)
{
}

int handle_syscall_close(int /*file*/)
{
	return -1;
}

int handle_syscall_execve(char * /*name*/, char ** /*argv*/, char ** /*env*/)
{
	return -1;
}

int handle_syscall_fork()
{
	return -1;
}

int handle_syscall_fstat(int /*file*/, struct stat * /*st*/)
{
	return -1;
}

int handle_syscall_getpid()
{
	return 0;
}

int handle_syscall_isatty(int /*file*/)
{
	return 0;
}

int handle_syscall_kill(int /*pid*/, int /*sig*/)
{
	return -1;
}

int handle_syscall_link(char * /*oldpath*/, char * /*newpath*/)
{
	return -1;
}

int handle_syscall_lseek(int /*file*/, int /*ptr*/, int /*dir*/)
{
	return -1;
}

int handle_syscall_open(const char * /*name*/, int /*flags*/, ...)
{
	return -1;
}

int handle_syscall_read(int /*file*/, char * /*ptr*/, int /*len*/)
{
	return -1;
}

caddr_t handle_syscall_sbrk(int /*incr*/)
{
	return (caddr_t)-1;
}

int handle_syscall_stat(const char * /*file*/, struct stat * /*st*/)
{
	return -1;
}

clock_t handle_syscall_times(struct tms * /*buf*/)
{
	return -1;
}

int handle_syscall_unlink(char * /*name*/)
{
	return -1;
}

int handle_syscall_wait(int * /*status*/)
{
	return -1;
}

int handle_syscall_write(int /*file*/, char *ptr, int len)
{
	for (int i = 0; i < len; i++)
	{
		terminal_putchar(ptr[i]);
	}
	return 0;
}

int handle_syscall_gettimeofday(struct timeval * /*p*/, void * /*z*/)
{
	return -1;
}

int handle_unknown_syscall(uint32_t syscall_id, uint32_t param1, uint32_t param2, uint32_t param3)
{
	printf("Unknown syscall %lu with parameters %lu, %lu, %lu", syscall_id, param1, param2, param3);
	return -1;
}

} // namespace
