#!/usr/bin/env bash

./build_files.sh

make debug

#
##objectfiles="./build_os/start_kernel.o ./build_os/kernel.o ./build_os/port_access.o ./build_os/cursor.o ./build_os/screen_control.o"
#objectfiles="./build_os/start_kernel.o"
#for file in $(find . -type f -name \*.o)
#do
#    if [[ "$file" != "./build_os/start_kernel.o" ]]; then
##        if [[ "$file" != "./build_os/kernel.o" ]]; then
#            objectfiles="$objectfiles $file"
##        fi
#    fi
#done
#
#set -x
#i386-elf-ld -o ./build_os/kernel.elf -Ttext 0x1000 $objectfiles
##i386-elf-ld -o ./build_os/kernel.elf -Ttext 0x1000 ./build_os/start_kernel.o ./build_os/kernel.o
#qemu-system-i386 -s -S -fda ./build_os/os.bin &
#/usr/local/i386elfgcc/bin/i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file ./build_os/kernel.elf"