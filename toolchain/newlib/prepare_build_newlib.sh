#!/bin/bash
set -o errexit
set -o xtrace

mkdir -p external
cd external

rm -rf newlib-2.2.0-1
tar xf /tmp/newlib-2.2.0-1.tar.gz

pushd newlib-2.2.0-1

# Step 1

patch -p1 < ../../modify-newlib.patch

pushd newlib/libc/sys
autoconf
popd

# Step 2

mkdir newlib/libc/sys/ost
cp ../../custom-src/* newlib/libc/sys/ost

pushd newlib/libc/sys
autoconf
popd

pushd newlib/libc/sys/ost
autoreconf
popd

popd

# Build

mkdir build-newlib
cd build-newlib
../newlib-2.2.0-1/configure --prefix=/usr --target=i686-elf "$@"

echo Configuration done, now run make
echo cd external/build-newlib
echo make all -j5
echo if building for i686-elf, run PATH=$PATH:/tmp/i686-elf/bin
echo either: make DESTDIR=/tmp/myos install  # for initial i686-elf
echo or    : make install                    # for i686-ost

# "For some reason, the newer versions of newlib (at least for me) didn't put the libraries in a location where other utilities like binutils could find. So here's another hack to fix this:"
echo cp -ar /tmp/myos/usr/i686-ost/* /tmp/myos/usr/

