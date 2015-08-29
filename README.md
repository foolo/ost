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

	cd toolchain
	./download_libs.sh

## Build i686-elf cross compiler

	cd i686-elf-cross-compiler
	./build_cross_compiler.sh

### Build Newlib 1

Go to newlib directory and run

	./configure-ost-newlib.sh
	./build-ost-newlib.sh $ELFCC/ost-links

### Step 2: Build Binutils

Go to hosted_compiler directory and run

	prepare_build_binutils.sh

and then run make according to instructions in prepare_build_binutils.sh

	prepare_build_gcc.sh


### Build Newlib 2

	./configure-ost-newlib.sh
	./build-ost-newlib.sh $OSTCC/bin

# Build kernel

	cd kernel
	make



# Libstdc++

Taken from http://www.linuxfromscratch.org/lfs/view/development/chapter05/gcc-libstdc++.html

	cd toolchain/hosted_compiler/external/build-gcc
	../gcc-4.9.2/libstdc++-v3/configure --enable-option-checking --target=i686-ost --prefix=$OSTCC --with-sysroot=$OSTROOT --enable-languages=c,c++ --disable-werror --disable-multilib --disable-nls  --disable-libstdcxx-threads --disable-libstdcxx-pch
	make
	sudo make install

Remove lines from /usr/local/ost/include/c++/4.9.2/bits/os_defines.h

	#if __GLIBC_PREREQ(2,15) && defined(_GNU_SOURCE)
	# undef _GLIBCXX_HAVE_GETS
	#endif



## Make a bootable cdrom .iso

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

