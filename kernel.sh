#!/usr/bin/env bash

set -x
rm -rf *.bin *.o
/usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -c ./kernel/kernel.c -o ./build_os/kernel.o # Main Kernal Code
nasm ./on_boot/start_kernel.s -f elf -o ./build_os/start_kernel.o # Pass over to 32 bit protected mode
/usr/local/i386elfgcc/bin/i386-elf-ld -o ./build_os/kernel.bin -Ttext 0x1000 ./build_os/start_kernel.o ./build_os/kernel.o --oformat binary # Convert Kernel and Convertto32bit code to one bin file -- KERNEL
cd on_boot
nasm boot_sector_main.s -f bin -o ../build_os/boot_sector_main.bin # Boot Sector code -- OS START
cd ..
cat ./build_os/boot_sector_main.bin ./build_os/kernel.bin > ./build_os/os.bin # Combine all code into one binary OS File
qemu-system-i386 -fda ./build_os/os.bin # Simulate OS