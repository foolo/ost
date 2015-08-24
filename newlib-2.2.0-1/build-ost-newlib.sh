#!/bin/bash
set -o verbose
set -o errexit

source ../toolchain/toochain_env.sh

if [ -z "$1" ]
then
	echo "missing CCPATH argument (e.g. /usr/local/x86_64-elf/ost-links, /usr/local/ost/bin)"
	exit
fi
CCPATH=$1
PATH=$PATH:$CCPATH

make all -j5

sudo echo "Entering sudo"
sudo ln -sf $CCPATH/x86_64-ost-ranlib /usr/local/bin/x86_64-ost-ranlib #needed for sudo to access x86_64-ost-ranlib
sudo make DESTDIR=$OSTROOT install
sudo rm -f /usr/local/bin/x86_64-ost-ranlib

pushd $OSTROOT/usr
sudo cp -ar x86_64-ost/* .
popd
