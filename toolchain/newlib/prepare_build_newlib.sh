#!/bin/bash
set -o errexit
set -o xtrace

source ../toochain_env.sh
export PATH=$ELFCC/bin:$PATH

if [ -z "$BINPATH" ]
then
	echo "BINPATH not set"
	exit
fi

mkdir -p external
cd external

rm -rf newlib-2.2.0-1
tar xf $DOWNLOADS/newlib-2.2.0-1.tar.gz

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

PATH=$PATH:$BINPATH

mkdir build-newlib
pushd build-newlib
../newlib-2.2.0-1/configure --prefix=/usr --target=i686-ost
make all
sudo ln -sf $BINPATH/i686-ost-ranlib /usr/local/bin/i686-ost-ranlib #needed for sudo to access i686-ost-ranlib
sudo make DESTDIR=$OSTROOT install
sudo rm -f /usr/local/bin/i686-ost-ranlib

pushd $OSTROOT/usr
sudo cp -ar i686-ost/* .
popd
