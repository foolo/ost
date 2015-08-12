#!/bin/bash

set -o errexit
set -o xtrace

source ../toochain_env.sh

mkdir -p external
cd external

rm -rf gcc-4.9.2

tar xf $DOWNLOADS/gcc-4.9.2.tar.gz
tar xf $DOWNLOADS/isl-0.12.2.tar.bz2
tar xf $DOWNLOADS/mpfr-3.1.2.tar.xz
tar xf $DOWNLOADS/cloog-0.18.1.tar.gz
tar xf $DOWNLOADS/gmp-6.0.0a.tar.xz
tar xf $DOWNLOADS/mpc-1.0.2.tar.gz

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
../gcc-4.9.2/configure --enable-option-checking --target=i686-ost --prefix=$OSTCC --with-sysroot=$OSTCC/sysroot --disable-multilib --disable-werror
make all-gcc all-target-libgcc
sudo make install-gcc install-target-libgcc
popd
