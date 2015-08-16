#!/bin/bash
set -o verbose
set -o errexit

source ../toolchain/toochain_env.sh

if [ -z "$1" ]
then
	echo "missing CCPATH argument (e.g. /usr/local/i686-elf/ost-links, /usr/local/ost/bin)"
	exit
fi
CCPATH=$1
PATH=$PATH:$CCPATH

make all

sudo echo "Entering sudo"
sudo ln -sf $CCPATH/i686-ost-ranlib /usr/local/bin/i686-ost-ranlib #needed for sudo to access i686-ost-ranlib
sudo make DESTDIR=$OSTROOT install
sudo rm -f /usr/local/bin/i686-ost-ranlib

pushd $OSTROOT/usr
sudo cp -ar i686-ost/* .
popd
