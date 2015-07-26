#!/bin/bash

set -o errexit
set -o xtrace

mkdir -p external
cd external

rm -rf gcc-4.9.2

tar xf /tmp/gcc-4.9.2.tar.gz


# Patch

pushd gcc-4.9.2
patch -p1 < ../../modify-gcc.patch
cp ../../gcc-custom-src/* gcc/config
popd

pushd gcc-4.9.2/libstdc++-v3
autoconf
popd

# Configure

rm -rf build-gcc
mkdir build-gcc
pushd build-gcc
../gcc-4.9.2/configure --enable-option-checking --target=i686-ost --prefix=/usr/local/ost --with-sysroot=/usr/local/ost/sysroot --disable-multilib --disable-werror
popd

pushd build-gcc
make all-gcc all-target-libgcc

#needed for sudo to access i686-ost-ranlib
echo sudo ln -s /home/olof/Applications/i686cc/bin/i686-elf-ranlib /usr/local/bin/i686-ost-ranlib
echo sudo make install-gcc install-target-libgcc
echo sudo rm /usr/local/bin/i686-ost-ranlib
echo popd
