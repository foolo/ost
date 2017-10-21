#!/bin/bash

set -o errexit
set -o xtrace

source ../toochain_env.sh

mkdir -p external
cd external

rm -rf binutils-2.24
tar xf ../../downloads/binutils-2.24.tar.gz


# Patch and configure binutils

pushd binutils-2.24
patch -p1 < ../../modify-binutils.patch
cp ../../binutils-custom-src/* ld/emulparams
popd

pushd binutils-2.24/ld
automake
popd

rm -rf build-binutils
mkdir build-binutils
pushd build-binutils
../binutils-2.24/configure --enable-option-checking --target=i686-ost --prefix=$OSTCC --disable-werror --with-sysroot
make -j5
sudo make install
popd
