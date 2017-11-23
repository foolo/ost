#pragma once

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>

void handle_syscall_exit(int file);
int handle_syscall_close(int file);
int handle_syscall_execve(char * name, char ** argv, char ** env);
int handle_syscall_fork();
int handle_syscall_fstat(int file, struct stat * st);
int handle_syscall_getpid();
int handle_syscall_isatty(int file);
int handle_syscall_kill(int pid, int sig);
int handle_syscall_link(char * oldpath, char * newpath);
int handle_syscall_lseek(int file, int ptr, int dir);
int handle_syscall_open(const char * name, int flags, ...);
int handle_syscall_read(int file, char * ptr, int len);
caddr_t handle_syscall_sbrk(int incr);
int handle_syscall_stat(const char * file, struct stat * st);
clock_t handle_syscall_times(struct tms * buf);
int handle_syscall_unlink(char * name);
int handle_syscall_wait(int * status);
int handle_syscall_write(int file, char *ptr, int len);
int handle_syscall_gettimeofday(struct timeval * p, void * z);
int handle_unknown_syscall(uint32_t syscall_id, uint32_t param1, uint32_t param2, uint32_t param3);
