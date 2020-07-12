#!/usr/bin/env bash
set -x

pwd

#sudo mkdir build_os
#pip install wheel
#pip install gdown
#
#export PATH=$PATH:/home/runner/.local/lib/python2.7/site-packages
#export PATH=$PATH:/home/runner/.local/bin
#pip show gdown
#gdown https://drive.google.com/uc?id=1oiMvSt-Y3L08lT29yT16EHU_EgEQchBL -O /tmp/i386-elf-gcc-ubuntu.tar.gz
#
#sudo tar xf /tmp/i386-elf-gcc-ubuntu.tar.gz -C /usr/local/

#sudo apt-get update
#sudo apt-get install curl -y
#sudo apt-get install gcc -y
#sudo apt-get install build-essential -y
#sudo apt-get install libgmp-dev -y
#sudo apt-get install libmpc-dev -y
#sudo apt-get install libmpfr-dev -y
#sudo apt-get install bison -y
#sudo apt-get install flex -y
#sudo apt-get install texinfo -y
#sudo apt-get install automake -y
#sudo apt-get install autotools-dev -y
#sudo apt-get install nasm -y
#sudo apt-get install valgrind -y
sudo apt-get update
sudo apt-get remove docker docker-engine docker.io
sudo apt install docker.io
sudo systemctl start docker
sudo systemctl enable docker
docker --version

docker pull sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest
#docker create -v $PWD:/src --name ubuntu-cross-compiler-git-action sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest
#docker start ubuntu-cross-compiler-git-action
#docker exec --workdir /src ubuntu-cross-compiler-git-action make os.iso
#docker exec --workdir /src ubuntu-cross-compiler-git-action make kernel.elf
#docker rm ubuntu-cross-compiler-git-action

#cd HarshOS
#docker build .
#set -x
#
#pwd
#
#sudo mkdir build_os
#pip install wheel
#pip install gdown
#
#export PATH=$PATH:/home/runner/.local/lib/python2.7/site-packages
#export PATH=$PATH:/home/runner/.local/bin
#pip show gdown
#gdown https://drive.google.com/uc?id=1oiMvSt-Y3L08lT29yT16EHU_EgEQchBL -O /tmp/i386-elf-gcc-ubuntu.tar.gz
#
#sudo tar xf /tmp/i386-elf-gcc-ubuntu.tar.gz -C /usr/local/
#
#sudo apt-get install curl -y
#sudo apt-get install gcc -y
#sudo apt-get install build-essential -y
#sudo apt-get install libgmp-dev -y
#sudo apt-get install libmpc-dev -y
#sudo apt-get install libmpfr-dev -y
#sudo apt-get install bison -y
#sudo apt-get install flex -y
#sudo apt-get install texinfo -y
#sudo apt-get install automake -y
#sudo apt-get install autotools-dev -y
#####sudo apt-get install nasm -y
#####sudo apt-get install valgrind -y

#export CC=gcc
#export LD=ld


#export PREFIX="/usr/local/i386elfgcc"
#export TARGET=i386-elf
#export PATH="$PREFIX/bin:$PATH"

#mkdir /tmp/src
#cd /tmp/src
#curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.gz # If the link 404's, look for a more recent version
#tar xf binutils-2.34.tar.gz
#mkdir binutils-build
#cd binutils-build
#pwd
#../binutils-2.34/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
#make all install 2>&1 | tee make.log
#
#cd /tmp/src
#pwd
##apt-cache policy libgmp-dev
##apt-cache policy libmpc-dev
##apt-cache policy libmpfr-dev
#curl -O https://ftp.gnu.org/gnu/gcc/gcc-10.1.0/gcc-10.1.0.tar.gz
#tar xf gcc-10.1.0.tar.gz
#mkdir gcc-build
#cd gcc-build
#pwd
#../gcc-10.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c --without-headers --with-mpfr=/usr/lib/x86_64-linux-gnu --with-gmp=/usr/lib/x86_64-linux-gnu --with-mpc=/usr/lib/x86_64-linux-gnu
#sudo make all-gcc
#sudo make all-target-libgcc
#sudo make install-gcc
#sudo make install-target-libgcc
#
#cd /tmp/src
#curl -O http://ftp.rediris.es/mirror/GNU/gdb/gdb-9.2.tar.gz
#tar xf gdb-9.2.tar.gz
#mkdir gdb-build
#cd gdb-build
#../gdb-9.2/configure --target="$TARGET" --prefix="$PREFIX" --program-prefix=i386-elf-
#sudo make
#sudo make install
#
#cd /usr/local
#tar -czvf /home/runner/work/kernel/kernel/i386-elf-gcc-ubuntu.tar.gz i386elfgcc
#cd /home/runner/work/kernel/kernel
#/usr/bin/git config --global user.email "harshasingam3@gmail.com"
#/usr/bin/git config --global user.name "Sriharsha Singam"
#/usr/bin/git add .
#/usr/bin/git commit -m adding-ubuntu-cross-compiler
#/usr/bin/git push
