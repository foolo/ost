#!/bin/bash

set -o errexit
set -o xtrace

wget --no-clobber --directory-prefix /tmp http://ftpmirror.gnu.org/binutils/binutils-2.24.tar.gz
wget --no-clobber --directory-prefix /tmp http://ftpmirror.gnu.org/gcc/gcc-4.9.2/gcc-4.9.2.tar.gz

wget --no-clobber --directory-prefix /tmp http://ftpmirror.gnu.org/mpfr/mpfr-3.1.2.tar.xz
wget --no-clobber --directory-prefix /tmp http://ftpmirror.gnu.org/gmp/gmp-6.0.0a.tar.xz
wget --no-clobber --directory-prefix /tmp http://ftpmirror.gnu.org/mpc/mpc-1.0.2.tar.gz

wget --no-clobber --directory-prefix /tmp ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.12.2.tar.bz2
wget --no-clobber --directory-prefix /tmp ftp://gcc.gnu.org/pub/gcc/infrastructure/cloog-0.18.1.tar.gz

wget --no-clobber --directory-prefix /tmp ftp://sourceware.org/pub/newlib/newlib-2.2.0-1.tar.gz
