#!/usr/bin/env bash



make clean
make os.iso


#set -e

#cd build_os
#rm -rf *.bin *.o *.elf
#cd ..
#
#cfiles=""
#for file in $(find . -type f -name \*.c)
#do
#    filename=${file%.c}
#    filename=${filename##*/}
##    cfiles="$cfiles $file"
#    /usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -O2 -s -c -g "$file" -o ./build_os/"$filename.o"
#    echo "Created ./build_os/$filename.o"
#done
##echo "/usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -c -g $cfiles -o ./build_os/overall.o"
##/usr/local/i386elfgcc/bin/i386-elf-gcc -ffreestanding -c -g $cfiles -o ./build_os/overall.o
##echo "Created ./build_os/overall.o"
#
#nasm ./on_boot/start_kernel.s -f elf -o ./build_os/start_kernel.o # Pass over to 32 bit protected mode
#echo "Created ./build_os/start_kernel.o"
#
#objectfiles="./build_os/start_kernel.o ./build_os/kernel.o ./build_os/port_access.o ./build_os/cursor.o ./build_os/screen_control.o"
##objectfiles="./build_os/start_kernel.o"
##for file in $(find . -type f -name \*.o)
##do
##    if [[ "$file" != "./build_os/start_kernel.o" ]]; then
###        if [[ "$file" != "./build_os/kernel.o" ]]; then
##            objectfiles="$objectfiles $file"
###        fi
##    fi
##done
#
#/usr/local/i386elfgcc/bin/i386-elf-ld -o ./build_os/kernel.bin -Ttext 0x1000 $objectfiles --oformat binary # Convert Kernel and Convertto32bit code to one bin file -- KERNEL
#echo "Created ./build_os/kernel.bin from these files: $objectfiles"
#
#cd on_boot
#nasm boot_sector_main.s -f bin -o ../build_os/boot_sector_main.bin # Boot Sector code -- OS START
#cd ..
#echo "Created ./build_os/boot_sector_main.bin"
#
#cat ./build_os/boot_sector_main.bin ./build_os/kernel.bin > ./build_os/os.bin # Combine all code into one binary OS File
#echo "Created ./build_os/os.bin from ./build_os/boot_sector_main.bin ./build_os/kernel.bin"