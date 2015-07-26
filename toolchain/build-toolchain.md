# Prerequisites

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


# Build

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

