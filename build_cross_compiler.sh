wget http://ftpmirror.gnu.org/binutils/binutils-2.24.tar.gz
wget http://ftpmirror.gnu.org/gcc/gcc-4.9.2/gcc-4.9.2.tar.gz

wget http://ftpmirror.gnu.org/mpfr/mpfr-3.1.2.tar.xz
wget http://ftpmirror.gnu.org/gmp/gmp-6.0.0a.tar.xz
wget http://ftpmirror.gnu.org/mpc/mpc-1.0.2.tar.gz

wget ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.12.2.tar.bz2
wget ftp://gcc.gnu.org/pub/gcc/infrastructure/cloog-0.18.1.tar.gz

for f in *.tar*; do tar xf $f; done

cd gcc-4.9.2
ln -s ../mpfr-3.1.2 mpfr
ln -s ../gmp-6.0.0 gmp
ln -s ../mpc-1.0.2 mpc
ln -s ../isl-0.12.2 isl
ln -s ../cloog-0.18.1 cloog
cd ..


## Setup

mkdir -p /home/olof/Applications/i686cc
export PATH=/home/olof/Applications/i686cc/bin:$PATH
MYARCH=i686-elf
MYPREFIX=/home/olof/Applications/i686cc



## Binutils

mkdir build-binutils
cd build-binutils
../binutils-2.24/configure --prefix=$MYPREFIX --target=$MYARCH --disable-multilib --disable-werror
make -j4
sudo make install
cd ..


## Gcc

mkdir -p build-gcc
cd build-gcc
../gcc-4.9.2/configure --target=$MYARCH --prefix=$MYPREFIX --enable-languages=c,c++ --disable-nls --enable-languages=c,c++ --without-headers
make -j4 all-gcc
make -j4 all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
cd ..

