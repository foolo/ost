#!/bin/bash
set -o verbose
set -o errexit

pushd newlib/libc/sys
autoconf
popd

pushd newlib/libc/sys/ost
autoreconf
popd

CFLAGS='-ffreestanding  -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2'

./configure --prefix=/usr --target=x86_64-ost --disable-newlib-supplied-syscalls CFLAGS='-mcmodel=large'

