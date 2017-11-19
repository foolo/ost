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
	return 0;
}

extern "C" int getpid()
{
	return 0;
}

extern "C" int isatty(int /*file*/)
{
	return 0;
}

extern "C" int kill(int /*pid*/, int /*sig*/)
{
	return 0;
}

extern "C" int link(char * /*old*/, char * /*new*/)
{


	return 0;
}

int lseek_binfile(int ptr);

extern "C" int lseek(int file, int ptr, int dir)
{
	if (file < 0 && dir == SEEK_SET) {
		return lseek_binfile(ptr);
	}
	return 0;
}

extern "C" int open(const char * /*name*/, int /*flags*/, ...)
{
	return 0;
}

int read_binfile(char *dst, int len);

extern "C" int read(int file, char *dst, int len) {
	if (file < 0) {
		return read_binfile(dst, len);
	}
	return 0;
}

extern "C" caddr_t sbrk(int /*incr*/)
{
	return 0;
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
