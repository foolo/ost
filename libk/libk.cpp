#include <stdio.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/types.h>


unsigned sleep(unsigned seconds)
{
	return seconds;
}

int fcntl(int fd, int cmd, ...)
{
	return 0;
}


int truncate(const char *path, off_t length)
{
	return -1;
}

int ftruncate(int fd, off_t length)
{
	return -1;
}

void flockfile(FILE *filehandle)
{
	return;
}

//int ftrylockfile(FILE *filehandle);

void funlockfile(FILE *filehandle)
{
	return;
}

FILE *popen(const char *command, const char *type)
{
	return NULL;
}

int pclose(FILE *stream)
{
	return -1;
}
