#!/bin/bash

set -o errexit
set -o xtrace

mkdir -p external
cd external

rm -rf *

tar xf /tmp/binutils-2.24.tar.gz
tar xf /tmp/gcc-4.9.2.tar.gz
tar xf /tmp/mpfr-3.1.2.tar.xz
tar xf /tmp/gmp-6.0.0a.tar.xz
tar xf /tmp/mpc-1.0.2.tar.gz
tar xf /tmp/isl-0.12.2.tar.bz2
tar xf /tmp/cloog-0.18.1.tar.gz

cd gcc-4.9.2
ln -s ../mpfr-3.1.2 mpfr
ln -s ../gmp-6.0.0 gmp
ln -s ../mpc-1.0.2 mpc
ln -s ../isl-0.12.2 isl
ln -s ../cloog-0.18.1 cloog
cd ..


## Setup
export PATH=/tmp/i686-elf/bin:$PATH


## Binutils

mkdir build-binutils
pushd build-binutils
../binutils-2.24/configure --enable-option-checking --target=i686-elf --prefix=/tmp/i686-elf --disable-multilib --disable-werror --disable-nls
popd

## Gcc

mkdir -p build-gcc
pushd build-gcc
../gcc-4.9.2/configure --enable-option-checking --target=i686-elf --prefix=/tmp/i686-elf
popd


pushd build-binutils
make
sudo make install
popd

pushd build-gcc
make all-gcc
make all-target-libgcc
sudo ln -fs /tmp/i686-elf/bin/i686-elf-ranlib /usr/local/bin/i686-elf-ranlib # Needed for install-target-libgcc, as sudo does not use PATH
sudo make install-gcc
sudo make install-target-libgcc
popd

# Fix for Newlib. Newlib expects "i686-ost" compiler, fix with some links
mkdir -p /tmp/i686-elf-ost-links
pushd /tmp/i686-elf-ost-links
ln -sr /tmp/i686-elf/bin/i686-elf-ar   i686-ost-ar
ln -sr /tmp/i686-elf/bin/i686-elf-as   i686-ost-as
ln -sr /tmp/i686-elf/bin/i686-elf-gcc  i686-ost-gcc
ln -sr /tmp/i686-elf/bin/i686-elf-gcc  i686-ost-cc # Newlib uses cc instead of gcc
ln -sr /tmp/i686-elf/bin/i686-elf-ranlib i686-ost-ranlib

