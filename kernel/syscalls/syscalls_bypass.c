#include <stdio.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/time.h>
#include "syscall-handler.h"

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

int read(int file, char *dst, int len) {
	return 0;
}

extern uint32_t kernel_heap_start;
extern uint32_t kernel_heap_end;
caddr_t current_break = (caddr_t)&kernel_heap_start;
caddr_t sbrk(int incr)
{
	caddr_t previous_break = current_break;
	if (current_break + incr > (caddr_t)&kernel_heap_end) {
		printf("kernel out of heap memory\n");
		while (1) {}
	}
	current_break += incr;
	return previous_break;
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
	return handle_syscall_write(file, ptr, len);
}

int gettimeofday(struct timeval *p, void *z)
{
	return 0;
}
