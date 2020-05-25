#!/usr/bin/env bash

apt-get install curl --force-yes
apt-get install gcc --force-yes
apt-get install build-essential --force-yes
apt-get install	libgmp3-dev --force-yes
apt-get install libmpc-dev --force-yes
apt-get install libmpfr-dev --force-yes

export CC=gcc
export LD=ld

export PREFIX="/usr/local/i386elfgcc"
export TARGET=i386-elf
export PATH="$PREFIX/bin:$PATH"

mkdir /tmp/src
cd /tmp/src
curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.gz # If the link 404's, look for a more recent version
tar xf binutils-2.34.tar.gz
mkdir binutils-build
cd binutils-build
pwd
../binutils-2.34/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
make all install 2>&1 | tee make.log

cd /tmp/src
pwd
curl -O https://ftp.gnu.org/gnu/gcc/gcc-10.1.0/gcc-10.1.0.tar.gz
tar xf gcc-10.1.0.tar.gz
mkdir gcc-build
cd gcc-build
pwd
../gcc-10.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
