#!/bin/bash

set -o errexit
set -o xtrace

mkdir -p external
cd external

rm -rf gcc-4.9.2

tar xf /tmp/gcc-4.9.2.tar.gz
tar xf /tmp/isl-0.12.2.tar.bz2
tar xf /tmp/mpfr-3.1.2.tar.xz
tar xf /tmp/cloog-0.18.1.tar.gz
tar xf /tmp/gmp-6.0.0a.tar.xz
tar xf /tmp/mpc-1.0.2.tar.gz

cd gcc-4.9.2
ln -s ../mpfr-3.1.2 mpfr
ln -s ../gmp-6.0.0 gmp
ln -s ../mpc-1.0.2 mpc
ln -s ../isl-0.12.2 isl
ln -s ../cloog-0.18.1 cloog
cd ..


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
