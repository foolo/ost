#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include "syscall-handler.h"

extern "C" void _exit()
{
	return;
}

extern "C" int close(int /*file*/)
{
	return 0;
}

// pointer to array of char * strings that define the current environment variables
char **environ;

extern "C" int execve(char * /*name*/, char ** /*argv*/, char ** /*env*/)
{
	return 0;
}

extern "C" int fork()
{
	return 0;
}

extern "C" int fstat(int /*file*/, struct stat * /*st*/)
{
	return -1;
}

extern "C" int getpid()
{
	return 1234;
}

extern "C" int isatty(int /*file*/)
{
	return 0; // 0 = no
}

extern "C" int kill(int /*pid*/, int /*sig*/)
{
	return -1;
}

extern "C" int link(char * /*old*/, char * /*new*/)
{
	return 0;
}

extern "C" int lseek(int /*file*/, int /*ptr*/, int /*dir*/)
{
	return 0;
}

extern "C" int open(const char * /*name*/, int /*flags*/, ...)
{
	return 0;
}

extern "C" int read(int /*file*/, char * /*ptr*/, int /*len*/)
{
	return 0;
}

static caddr_t current_break_address = (caddr_t)0x300000;
extern "C" caddr_t sbrk(int incr)
{
	caddr_t break_address_before_increase = current_break_address;
	if (incr > 0) {
		current_break_address += incr;
	}
	return break_address_before_increase;
}

extern "C" int stat(const char * /*file*/, struct stat * /*st*/)
{
	return 0;
}

extern "C" clock_t times(struct tms * /*buf*/)
{
	return 0;
}

extern "C" int unlink(char * /*name*/)
{
	return 0;
}

extern "C" int wait(int * /*status*/)
{
	return 0;
}

extern "C" int write(int file, char *ptr, int len)
{
	return kernel::handle_syscall_write(file, ptr, len);
}

extern "C" int gettimeofday(struct timeval * /*p*/, void * /*z*/)
{
	return 0;
}
