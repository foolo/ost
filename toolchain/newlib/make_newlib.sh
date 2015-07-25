#!/bin/bash
set -o errexit
set -o xtrace

# Build

TMPLINKDIR=/tmp/i686-elf-ost-links
PATH=$PATH:$TMPLINKDIR
pushd external/build-newlib
make all
make DESTDIR=/tmp/myos install  #for initial i686-elf version
