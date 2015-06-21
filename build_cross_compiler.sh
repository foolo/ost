wget http://ftpmirror.gnu.org/binutils/binutils-2.24.tar.gz
wget http://ftpmirror.gnu.org/gcc/gcc-4.9.2/gcc-4.9.2.tar.gz

wget http://ftpmirror.gnu.org/mpfr/mpfr-3.1.2.tar.xz
wget http://ftpmirror.gnu.org/gmp/gmp-6.0.0a.tar.xz
wget http://ftpmirror.gnu.org/mpc/mpc-1.0.2.tar.gz

for f in *.tar*; do tar xf $f; done

cd gcc-4.9.2
ln -s ../mpfr-3.1.2 mpfr
ln -s ../gmp-6.0.0 gmp
ln -s ../mpc-1.0.2 mpc
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
../gcc-4.9.2/configure --prefix=$MYPREFIX --target=$MYARCH --enable-languages=c,c++ --disable-multilib
make -j4 all-gcc
sudo make install-gcc
cd ..

