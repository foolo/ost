# Installing the toolchain

## Install autoconf 2.64

	sudo apt-get install m4
	wget http://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.xz

Extract, configure, make, make install

## Install automake 1.11.1

	wget http://ftp.gnu.org/gnu/automake/automake-1.11.1.tar.bz2

Extract, configure, make, make install

## Install libtool

	sudo apt-get install libtool-bin

## Install texinfo

	sudo apt-get install texinfo

## Download GNU libraries

./download_libs.sh

## Build i686-elf cross compiler

run build_cross_compiler.sh and follow instructions for make
it will place executables in /tmp/i686-elf/bin

### Build Newlib 1

Go to newlib directory and run

	bash
	BINPATH=/tmp/i686-elf-ost-links ./prepare_build_newlib.sh

### Step 2: Build Binutils

Go to hosted_compiler directory and run

	prepare_build_binutils.sh

and then run make according to instructions in prepare_build_binutils.sh



prepare_build_gcc.sh


### Build Newlib 2

	bash
	BINPATH=/usr/local/ost/bin ./prepare_build_newlib.sh



# Make

	make

# Make a bootable cdrom .iso

Install prerequisites

	sudo apt-get install xorriso

Build the iso

	make myiso.iso

# Run

	qemu-system-i386 -kernel myos.bin

or for .iso files

	qemu-system-i386 -cdrom myos.iso


# Notes

The following integer symbols are defined in the Eclipse project.
They are used by stdint-gcc.h

__INT8_TYPE__ signed char
__UINT8_TYPE__ unsigned char
__INT16_TYPE__ short
__UINT16_TYPE__ unsigned short
__INT32_TYPE__ long
__UINT32_TYPE__ unsigned long
__INT64_TYPE__ long long
__INT64_TYPE__ unsigned long long

