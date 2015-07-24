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

#mkdir -p /tmp/i686-elf
#export PATH=/tmp/i686-elf/bin:$PATH



## Binutils

mkdir build-binutils
pushd build-binutils
../binutils-2.24/configure --prefix=/tmp/i686-elf --target=i686-elf --with-sysroot --disable-multilib --disable-werror --disable-nls
popd

## Gcc

mkdir -p build-gcc
pushd build-gcc
../gcc-4.9.2/configure --target=i686-elf --prefix=/tmp/i686-elf --enable-languages=c,c++ --disable-nls --enable-languages=c,c++ --without-headers
popd


echo pushd external/build-binutils
echo make -j5
echo sudo make install
echo popd



echo pushd external/build-gcc
echo make -j5 all-gcc
echo make -j5 all-target-libgcc
echo sudo make install-gcc
echo sudo make install-target-libgcc
echo popd

echo sudo chown olof /tmp/i686-elf
echo cd /tmp/i686-elf/bin
echo ln i686-elf-gcc i686-ost-cc
echo ln -s i686-elf-gcc-ar i686-ost-ar

sudo ln i686-elf-ar i686-ost-ar
sudo ln i686-elf-as i686-ost-as
sudo ln i686-elf-gcc i686-ost-gcc
sudo ln i686-elf-gcc i686-ost-cc
sudo ln i686-elf-ranlib i686-ost-ranlib
