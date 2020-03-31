#!/usr/bin/env bash

set -x
rm -rf *.bin *.o
/usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -c kernel.c -o kernel.o # Main Kernal Code
nasm start_kernel.s -f elf -o start_kernel.o # Pass over to 32 bit protected mode
/usr/local/i386elfgcc/bin/i386-elf-ld -o kernel.bin -Ttext 0x1000 start_kernel.o kernel.o --oformat binary # Convert Kernel and Convertto32bit code to one bin file -- KERNEL
nasm boot_sector_main.s -f bin -o boot_sector_main.bin # Boot Sector code -- OS START
cat boot_sector_main.bin kernel.bin > os.bin # Combine all code into one binary OS File
qemu-system-i386 -fda os.bin # Simulate OS