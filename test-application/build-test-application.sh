#!/bin/bash

set -o errexit
set -o xtrace

PATH=$PATH:/usr/local/ost/bin

# TODO why is "-m elf_i386" needed?
LDFLAGS=-Wl,-m,elf_i386
rm -f a.out
i686-ost-gcc $LDFLAGS test-application.cpp
objcopy -O binary a.out binfile
xxd -i binfile > ../kernel/binfile.cpp