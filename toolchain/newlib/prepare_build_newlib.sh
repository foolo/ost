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
ln  ../../i686-ost/* newlib/libc/sys/ost/

pushd newlib/libc/sys
autoconf
popd

pushd newlib/libc/sys/ost
autoreconf
popd

popd

# Build

PATH=$PATH:/tmp/i686-elf/bin


# Newlib expects "i686-ost" compiler, fix with some links
TMPLINKDIR=/tmp/i686-elf-ost-links
rm -rf $TMPLINKDIR
mkdir -p $TMPLINKDIR
pushd $TMPLINKDIR
ln -s /tmp/i686-elf/bin/i686-elf-ar   i686-ost-ar
ln -s /tmp/i686-elf/bin/i686-elf-as   i686-ost-as
ln -s /tmp/i686-elf/bin/i686-elf-gcc  i686-ost-gcc
ln -s /tmp/i686-elf/bin/i686-elf-gcc  i686-ost-cc # Newlib uses cc instead of gcc
ln -s /tmp/i686-elf/bin/i686-elf-ranlib i686-ost-ranlib
popd
PATH=$PATH:$TMPLINKDIR


mkdir build-newlib
pushd build-newlib
../newlib-2.2.0-1/configure --prefix=/usr --target=i686-ost
make all
make DESTDIR=/tmp/myos install  #for initial i686-elf version

# for i686-ost version
# make install

# "For some reason, the newer versions of newlib (at least for me) didn't put the libraries in a location where other utilities like binutils could find. So here's another hack to fix this:"
#echo cp -ar /tmp/myos/usr/i686-ost/* /tmp/myos/usr/

