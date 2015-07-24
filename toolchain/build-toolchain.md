## Install autoconf 2.64

	sudo apt-get install m4
	wget http://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.xz

Extract, configure, make, make install

## Install automake 1.11.1

	wget http://ftp.gnu.org/gnu/automake/automake-1.11.1.tar.bz2

Extract, configure, make, make install

## Install libtool
sudo apt-get install libtool-bin

### Build Newlib

Go to newlib directory and run prepare_build_newlib.sh

## Download GNU libraries

./download_libs.sh

### Step 2: Build Binutils

Go to hosted_compiler directory and run

prepare_build_binutils.sh

and then run make according to instructions in prepare_build_binutils.sh



prepare_build_gcc.sh
