#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

void _exit()
{
}

int close(int file)
{
}

// pointer to array of char * strings that define the current environment variables
char **environ;

int execve(char *name, char **argv, char **env)
{
}

int fork()
{
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
	return 0;
}

caddr_t sbrk(int incr)
{
	return 0;
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
	return 0;
}

int gettimeofday(struct timeval *p, void *z)
{
	return 0;
}
