#!/bin/bash
set -o verbose
set -o errexit

pushd newlib/libc/sys
autoconf
popd

pushd newlib/libc/sys/ost
autoreconf
popd

./configure --prefix=/usr --target=i686-ost
