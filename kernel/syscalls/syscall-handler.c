#include <stdio.h>
#include "syscall-handler.h"
#include "terminal.h"
#include "process/process.h"
#include "memory/ia32/paging.h"
#include "keyboard/input_queue.h"

void handle_syscall_exit(int file)
{
}

int handle_syscall_close(int file)
{
	return -1;
}

int handle_syscall_execve(char *name, char **argv, char **env)
{
	return -1;
}

int handle_syscall_fork()
{
	return -1;
}

int handle_syscall_fstat(int file, struct stat *st)
{
	return -1;
}

int handle_syscall_getpid()
{
	return 0;
}

int handle_syscall_isatty(int file)
{
	return 0;
}

int handle_syscall_kill(int pid, int sig)
{
	return -1;
}

int handle_syscall_link(char *oldpath, char *newpath)
{
	return -1;
}

int handle_syscall_lseek(int file, int ptr, int dir)
{
	return -1;
}

int handle_syscall_open(const char *name, int flags, ...)
{
	return -1;
}

int handle_syscall_read(int file, char *ptr, int len)
{
	//printf("handle_syscall_read\n");
	if (file == 0) {
		ptr[0] = input_queue_pop();
		return 1;
	}
	return -1;
}

caddr_t handle_syscall_sbrk(int incr)
{
	struct process_info *pr = get_current_process_info();
	void *previous_break = pr->current_break;
	set_up_userspace_page_tables(pr->pgdir, (uint32_t)pr->current_break, incr);
	pr->current_break += incr;
	return previous_break;
}

int handle_syscall_stat(const char *file, struct stat *st)
{
	return -1;
}

clock_t handle_syscall_times(struct tms *buf)
{
	return -1;
}

int handle_syscall_unlink(char *name)
{
	return -1;
}

int handle_syscall_wait(int * status)
{
	return -1;
}

int handle_syscall_write(int file, char *ptr, int len)
{
	for (int i = 0; i < len; i++)
	{
		terminal_putchar(ptr[i]);
	}
	return 0;
}

int handle_syscall_gettimeofday(struct timeval * p, void *z)
{
	return -1;
}

int handle_unknown_syscall(uint32_t syscall_id, uint32_t param1, uint32_t param2, uint32_t param3)
{
	printf("Unknown syscall %u with parameters %u, %u, %u", syscall_id, param1, param2, param3);
	return -1;
}
