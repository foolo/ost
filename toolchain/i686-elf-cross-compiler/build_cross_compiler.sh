#!/bin/bash

set -o errexit
set -o xtrace

source ../toochain_env.sh

mkdir -p external
cd external

rm -rf *

tar xf $DOWNLOADS/binutils-2.24.tar.gz
tar xf $DOWNLOADS/gcc-4.9.2.tar.gz
tar xf $DOWNLOADS/mpfr-3.1.2.tar.xz
tar xf $DOWNLOADS/gmp-6.0.0a.tar.xz
tar xf $DOWNLOADS/mpc-1.0.2.tar.gz
tar xf $DOWNLOADS/isl-0.12.2.tar.bz2
tar xf $DOWNLOADS/cloog-0.18.1.tar.gz

cd gcc-4.9.2
ln -s ../mpfr-3.1.2 mpfr
ln -s ../gmp-6.0.0 gmp
ln -s ../mpc-1.0.2 mpc
ln -s ../isl-0.12.2 isl
ln -s ../cloog-0.18.1 cloog
cd ..


## Setup
export PATH=$ELFCC/bin:$PATH


## Binutils

mkdir build-binutils
pushd build-binutils
../binutils-2.24/configure --enable-option-checking --target=i686-elf --prefix=$ELFCC --disable-multilib --disable-werror --disable-nls
popd

## Gcc

mkdir -p build-gcc
pushd build-gcc
../gcc-4.9.2/configure --enable-option-checking --target=i686-elf --prefix=$ELFCC
popd


pushd build-binutils
make
sudo make install
popd

pushd build-gcc
make all-gcc
make all-target-libgcc
sudo ln -fs $ELFCC/bin/i686-elf-ranlib /usr/local/bin/i686-elf-ranlib # Needed for install-target-libgcc, as sudo does not use PATH
sudo make install-gcc
sudo make install-target-libgcc
popd

# Fix for Newlib. Newlib expects "i686-ost" compiler, fix with some links
sudo mkdir -p $ELFCC/ost-links
pushd $ELFCC/ost-links
sudo ln -sr $ELFCC/bin/i686-elf-ar   i686-ost-ar
sudo ln -sr $ELFCC/bin/i686-elf-as   i686-ost-as
sudo ln -sr $ELFCC/bin/i686-elf-gcc  i686-ost-gcc
sudo ln -sr $ELFCC/bin/i686-elf-gcc  i686-ost-cc # Newlib uses cc instead of gcc
sudo ln -sr $ELFCC/bin/i686-elf-ranlib i686-ost-ranlib

