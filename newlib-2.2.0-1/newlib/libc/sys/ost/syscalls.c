#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include "syscalls.h"

void _exit()
{
}

int close(int file)
{
	return 0;
}

// pointer to array of char * strings that define the current environment variables
char **environ;

int execve(char *name, char **argv, char **env)
{
	return 0;
}

int fork()
{
	return 0;
}

int fstat(int file, struct stat *st)
{
	return 0;
}

int getpid()
{
	return 0;
}

int isatty(int file)
{
	return 0;
}

int kill(int pid, int sig)
{
	return 0;
}

int link(char *old, char *new)
{
	return 0;
}

int lseek(int file, int ptr, int dir)
{
	return 0;
}

int open(const char *name, int flags, ...)
{
	return 0;
}

int read(int file, char *ptr, int len)
{
	int ret;
	asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(SYSCALL_READ), "b"(file), "c"(ptr), "d"(len)
        : "cc", "edi", "esi", "memory"
    );
	return ret;
}

caddr_t sbrk(int incr)
{
	int ret;
	asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(SYSCALL_SBRK), "b"(incr)
        : "cc", "edi", "esi", "memory"
    );
	return ret;
}

int stat(const char *file, struct stat *st)
{
	return 0;
}

clock_t times(struct tms *buf)
{
	return 0;
}

int unlink(char *name)
{
	return 0;
}

int wait(int *status)
{
	return 0;
}

int write(int file, char *ptr, int len)
{
	int ret;
	asm volatile
    (
        "int $0x80"
        : "=a" (ret)
        : "0"(SYSCALL_WRITE), "b"(file), "c"(ptr), "d"(len)
        : "cc", "edi", "esi", "memory"
    );
	return 0;
}

int gettimeofday(struct timeval *p, void *z)
{
	return 0;
}
