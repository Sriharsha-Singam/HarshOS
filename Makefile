C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c helper/*.c memory/*.c initrd/harshfs.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.c helper/*.h memory/*.h initrd/harshfs.h)
BUILD_DIR = build_os
current_dir = $(shell pwd)
CURDIR := /src/HarshOS
# Nice syntax for file extension replacement
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SOURCES)))

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra #-Werror
LDFLAGS=-Tlink.ld

# objdump -D -Mintel,i8086 -b binary -m i8086 ./build_os/os.iso

################################################################################################################################################################
# Getting Size and Sector Values:
KERNEL_C_BYTES_SIZE = $$(wc -c < 'build_os/kernel.bin') # Get how many bytes the kernel takes up
NUMBER_OF_KERNEL_SECTORS = $$((($(KERNEL_C_BYTES_SIZE)+0x1FF)/0x200)) # Get the number of sectors that the kernel takes up.

HARSHFS_BYTES_SIZE = $$(wc -c < 'build_os/harshfs_kernel_initial_image.bin') # Get how many bytes the kernel takes up
HARSHFS_PAD = $((((($HARSHFS_BYTES_SIZE/512) + 1)*512)-($HARSHFS_BYTES_SIZE)))
NUMBER_OF_HARSHFS_SECTORS = $$((($(HARSHFS_BYTES_SIZE)+0x1FF)/0x200)) # Get the number of sectors that the kernel takes up.

OVERALL_BYTES_SIZE = $$(( $(KERNEL_C_BYTES_SIZE) + $(HARSHFS_BYTES_SIZE) ))
OVERALL_NUMBER_OF_SECTORS = $$(( $(NUMBER_OF_KERNEL_SECTORS) + $(NUMBER_OF_HARSHFS_SECTORS) ))

SECONDARY_BOOTSECTOR_SIZE = $$(wc -c < 'build_os/second_stage_bootsector.bin') # Get how many bytes the secondary bootsector takes up
NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS = $$((($(SECONDARY_BOOTSECTOR_SIZE)+0x1FF)/0x200)) # Get the number of sectors that the secondary bootsector takes up.
################################################################################################################################################################

.PHONY: all
all: os.iso

.PHONY: build_os
build_os:
	mkdir -p build_os

# First rule is run by default
.PHONY: os.iso
os.iso: clean build_os kernel_initrd_run build_os/boot_sector_main.bin build_os/second_stage_bootsector.bin kernel.bin kernel.elf
	echo "Secondary Bootsector Size: ${SECONDARY_BOOTSECTOR_SIZE}"
	echo "Number of Secondary Bootsector Sectors: ${NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS}"
	echo "Kernel Size: ${KERNEL_C_BYTES_SIZE}"
	echo "Number of Kernel Sectors: ${NUMBER_OF_KERNEL_SECTORS}"
	echo "Kernel + Initrd Size: ${OVERALL_BYTES_SIZE}"
	echo "Number of Kernel + Initrd  Sectors: ${OVERALL_NUMBER_OF_SECTORS}"
	echo "Initrd Number of Sectors: ${NUMBER_OF_HARSHFS_SECTORS}"
	cat build_os/boot_sector_main.bin build_os/second_stage_bootsector.bin build_os/kernel.bin build_os/harshfs_kernel_initial_image.bin > ./build_os/os.iso
	#ld -m elf_i386 -r build_os/boot_sector_main.elf build_os/kernel.elf -o build_os/harshos.elf
	cp ./build_os/os.iso ./final.img


# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
.PHONY: kernel.bin
kernel.bin: build_os/start_kernel.o build_os/interrupt.o ${OBJ}
	${LD} ${LDFLAGS} -o ./build_os/kernel.bin $^ --oformat binary

# Used for debugging purposes
.PHONY: kernel.elf
kernel.elf: build_os/start_kernel.o build_os/interrupt.o ${OBJ}
	${LD} ${LDFLAGS} -o ./build_os/kernel.elf $^

.PHONY: run
run: os.iso
	qemu-system-i386 -m 8G -fda build_os/os.iso

.PHONY: run-curses
run-curses: os.iso
	qemu-system-i386 -curses -fda build_os/os.iso

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: debug
debug: os.iso kernel.elf
	qemu-system-i386 -s -S -fda ./build_os/os.iso &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ./build_os/kernel.elf"

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: debug-curses
debug-curses: os.iso kernel.elf
	qemu-system-i386 -curses -s -S -fda ./build_os/os.iso &

# Generic rules for wildcards
# To make an object, always compile from its .c
$(BUILD_DIR)/%.o: kernel/%.c
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: initrd/%.c
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: drivers/%.c drivers/%.h
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: helper/%.c helper/%.h
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: memory/%.c memory/%.h
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: on_boot/%.s
	nasm $< -f elf -o $@

$(BUILD_DIR)/%.o: cpu/%.c
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: cpu/%.s
	nasm $< -f elf -o $@

.PHONY: kernel_initrd
kernel_initrd: build_os
	/usr/bin/gcc -g initrd/create_kernel_initrd.c -o build_os/create_kernel_initrd
	chmod +x build_os/create_kernel_initrd

.PHONY: kernel_initrd_run
kernel_initrd_run: kernel_initrd
	valgrind --leak-check=full --show-leak-kinds=all build_os/create_kernel_initrd
	TEST=$$(((((($$(wc -c < build_os/harshfs_kernel_initial_image.bin))/512) + 1)*512)-($$(wc -c < build_os/harshfs_kernel_initial_image.bin))))
	echo "HEHEHEHE: ${TEST}"
	dd if=/dev/zero bs=1 count=$$(((((($$(wc -c < build_os/harshfs_kernel_initial_image.bin))/512) + 1)*512)-($$(wc -c < build_os/harshfs_kernel_initial_image.bin))))>> build_os/harshfs_kernel_initial_image.bin

.PHONY: kernel_initrd_debug
kernel_initrd_debug: kernel_initrd
	gdbserver :1234 /src/HarshOS/build_os/create_kernel_initrd

$(BUILD_DIR)/second_stage_bootsector.bin:
	nasm -f elf32 on_boot/second_stage_bootsector.s -o build_os/second_stage_bootsector.elf
	nasm -f bin on_boot/second_stage_bootsector.s -o build_os/second_stage_bootsector.bin

$(BUILD_DIR)/boot_sector_main.bin: kernel.bin build_os/second_stage_bootsector.bin
	nasm -f elf32 -dNUMBER_OF_KERNEL_SECTORS=$(OVERALL_NUMBER_OF_SECTORS) -dKERNEL_C_BYTES_SIZE=$(OVERALL_BYTES_SIZE) -dSECONDARY_BOOTSECTOR_SIZE=$(SECONDARY_BOOTSECTOR_SIZE) -dNUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS=$(NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS) -dNUMBER_OF_HARSHFS_SECTORS=$(NUMBER_OF_HARSHFS_SECTORS) -dHARSHFS_BYTES_SIZE=$(HARSHFS_BYTES_SIZE) -dNUMBER_OF_KERNEL_SECTORS=$(NUMBER_OF_KERNEL_SECTORS) on_boot/boot_sector_main.s -o build_os/boot_sector_main.o
	ld -m elf_i386 -Ttext 0x7c00 build_os/boot_sector_main.o -o build_os/boot_sector_main.elf
	nasm -f bin -dBIN=1 -dNUMBER_OF_KERNEL_SECTORS=$(OVERALL_NUMBER_OF_SECTORS) -dKERNEL_C_BYTES_SIZE=$(OVERALL_BYTES_SIZE) -dSECONDARY_BOOTSECTOR_SIZE=$(SECONDARY_BOOTSECTOR_SIZE) -dNUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS=$(NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS) -dNUMBER_OF_HARSHFS_SECTORS=$(NUMBER_OF_HARSHFS_SECTORS) -dHARSHFS_BYTES_SIZE=$(HARSHFS_BYTES_SIZE) -dNUMBER_OF_KERNEL_SECTORS=$(NUMBER_OF_KERNEL_SECTORS) on_boot/boot_sector_main.s -o $@

.PHONY: clean
clean:
	rm -rf build_os/*.bin build_os/*.dis build_os/*.o build_os/os.iso build_os/*.elf
	rm -rf build_os/*.o build_os/*.bin build_os/*.o
	rm -rf build_os/*.*
	rm -f build_os/create_kernel_initrd

.PHONY: github-actions-os-iso
github-actions-os-iso:
	sudo docker run --rm --privileged --name harshos-github-actions -v "$(current_dir):/src" -w /src sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest make os.iso