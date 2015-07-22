#!/bin/bash

set -o errexit
set -o xtrace


mkdir -p external
cd external

wget --no-clobber http://ftpmirror.gnu.org/binutils/binutils-2.24.tar.gz
wget --no-clobber http://ftpmirror.gnu.org/gcc/gcc-4.9.2/gcc-4.9.2.tar.gz

wget --no-clobber http://ftpmirror.gnu.org/mpfr/mpfr-3.1.2.tar.xz
wget --no-clobber http://ftpmirror.gnu.org/gmp/gmp-6.0.0a.tar.xz
wget --no-clobber http://ftpmirror.gnu.org/mpc/mpc-1.0.2.tar.gz

wget --no-clobber ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.12.2.tar.bz2
wget --no-clobber ftp://gcc.gnu.org/pub/gcc/infrastructure/cloog-0.18.1.tar.gz


rm -rf binutils-2.24
rm -rf gcc-4.9.2

for f in *.tar*; do
	echo Extracting $f;
	tar xf $f;
done


# Patch and configure binutils


pushd binutils-2.24
patch -p1 < ../../modify-binutils.patch
cp ../../binutils-custom-src/* ld/emulparams
popd

pushd binutils-2.24/ld
automake
popd

# Patch and configure gcc

pushd gcc-4.9.2
patch -p1 < ../../modify-gcc.patch
cp ../../gcc-custom-src/* gcc/config
popd

pushd gcc-4.9.2/libstdc++-v3
autoconf
popd


rm -rf build-binutils
mkdir build-binutils
pushd build-binutils
../binutils-2.24/configure --target=i686-ost --prefix=/myos/usr --with-sysroot=/myos/sysroot --disable-werror
popd

rm -rf build-gcc
mkdir build-gcc
pushd build-gcc
../gcc-4.9.2/configure --target=i686-ost --prefix=/myos/usr --with-sysroot=/myos/sysroot --disable-werror
popd

echo Configuration done, now make binutils and gcc
echo pushd build-binutils
echo make
echo sudo make install
echo popd

echo pushd build-gcc
echo make all-gcc all-target-libgcc
echo sudo make install-gcc install-target-libgcc


