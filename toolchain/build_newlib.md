## Download sources

	wget ftp://sourceware.org/pub/newlib/newlib-2.2.0-1.tar.gz
	tar xf newlib-2.2.0-1.tar.gz
	cd newlib-2.2.0-1
	git init
	git add *
	git commit -m initial

## Edit newlib-2.2.0-1/config.sub

	# First accept the basic system types.
	...
	      | -aos* | -aros* | -ost* \


## Edit newlib-2.2.0-1/newlib/configure.host

Around line 365

	...
	# Get the source directories to use for the host.  unix_dir is set
	...
	case "${host}" in
	  i[3-7]86-*-ost*)
	    sys_dir=ost
	    ;;
	...

## Edit newlib-2.2.0-1/newlib/libc/sys/configure.in

Around line 25

	...
	if test -n "${sys_dir}"; then
	  case ${sys_dir} in
		ost) AC_CONFIG_SUBDIRS(ost) ;;
	...

## Run autoconf (exactly version 2.64)

	pushd newlib/libc/sys
	autoconf
	popd

## Create OS specific files

	mkdir -p newlib/libc/sys/ost

### Create newlib/libc/sys/ost/crt0.c

	#include <fcntl.h>
	 
	extern void exit(int code);
	extern int main ();
	 
	void _start() {
	    int ex = main();
	    exit(ex);
	}

TODO: add in argc and argv support based on how you handle them in your OS

### Create newlib/libc/sys/ost/syscalls.c

	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/fcntl.h>
	#include <sys/times.h>
	#include <sys/errno.h>
	#include <sys/time.h>
	#include <stdio.h>
	 
	void _exit();
	int close(int file);
	char **environ; /* pointer to array of char * strings that define the current environment variables */
	int execve(char *name, char **argv, char **env);
	int fork();
	int fstat(int file, struct stat *st);
	int getpid();
	int isatty(int file);
	int kill(int pid, int sig);
	int link(char *old, char *new);
	int lseek(int file, int ptr, int dir);
	int open(const char *name, int flags, ...);
	int read(int file, char *ptr, int len);
	caddr_t sbrk(int incr);
	int stat(const char *file, struct stat *st);
	clock_t times(struct tms *buf);
	int unlink(char *name);
	int wait(int *status);
	int write(int file, char *ptr, int len);
	int gettimeofday(struct timeval *p, struct timezone *z);

Complete the file with implementations of all the functions

### Create newlib/libc/sys/ost/configure.in

	AC_PREREQ(2.59)
	AC_INIT([newlib], [NEWLIB_VERSION])
	AC_CONFIG_SRCDIR([crt0.c])
	AC_CONFIG_AUX_DIR(../../../..)
	NEWLIB_CONFIGURE(../../..)
	AC_CONFIG_FILES([Makefile])
	AC_OUTPUT

### Create newlib/libc/sys/ost/Makefile.am

	AUTOMAKE_OPTIONS = cygnus
	INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)
	AM_CCASFLAGS = $(INCLUDES)
	 
	noinst_LIBRARIES = lib.a
	 
	if MAY_SUPPLY_SYSCALLS
	extra_objs = syscalls.o # add more object files here if you split up
	else                    # syscalls.c into multiple files in the previous step
	extra_objs =
	endif
	 
	lib_a_SOURCES =
	lib_a_LIBADD = $(extra_objs)
	EXTRA_lib_a_SOURCES = syscalls.c crt0.c # add more source files here if you split up
	lib_a_DEPENDENCIES = $(extra_objs)      # syscalls.c into multiple files
	lib_a_CCASFLAGS = $(AM_CCASFLAGS)
	lib_a_CFLAGS = $(AM_CFLAGS)
	 
	if MAY_SUPPLY_SYSCALLS
	all: crt0.o
	endif
	 
	ACLOCAL_AMFLAGS = -I ../../..
	CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host

## Run autoconf again

	pushd newlib/libc/sys
	autoconf
	popd

## Run autoreconf
	
	pushd newlib/libc/sys/ost
	autoreconf
	popd

## Fix temporary i686-ost-* links

	cd <dir of i686-elf-*>
	sudo ln -s i686-elf-ar i686-ost-ar
	sudo ln -s i686-elf-as i686-ost-as
	sudo ln -s i686-elf-gcc i686-ost-gcc
	sudo ln -s i686-elf-gcc i686-ost-cc
	sudo ln -s i686-elf-ranlib i686-ost-ranlib

## Create sysroot

	sudo mkdir /myos
	sudo chown olof /myos
	sudo chgrp olof /myos
	chmod 775 /myos

## Build

	mkdir build-newlib
	cd build-newlib
	../newlib-2.2.0-1/configure --prefix=/usr --target=i686-ost
	make all
	make DESTDIR=/myos install

Note: If `make all` fails because it cannot find libc/sys/ost/lib.a and
libc/sys/ost/crt0.o, go to the source dir toolchain/newlib-2.2.0-1/newlib/libc/sys/ost
and run ./configure and make. It will create lib.a and crt0.o. Copy them manually to
toolchain/build-newlib/i686-ost/newlib/libc/sys/ost, and run `make all` again.

According to tutorial:
"For some reason, the newer versions of newlib (at least for me) didn't put the libraries in a location where other utilities like binutils could find. So here's another hack to fix this:"

	cp -ar /myos/usr/i686-ost/* /myos/usr/