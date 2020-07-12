C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c helper/*.c memory/*.c initrd/harshfs.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.c helper/*.h memory/*.h initrd/harshfs.h)
BUILD_DIR = build_os
CURDIR := /src/HarshOS
# Nice syntax for file extension replacement
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SOURCES)))

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
LD = /usr/local/i386elfgcc/bin/i386-elf-ld
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror
LDFLAGS=-Tlink.ld

################################################################################################################################################################
# Getting Size and Sector Values:
KERNEL_C_BYTES_SIZE = $$(wc -c < 'build_os/kernel.bin') # Get how many bytes the kernel takes up
NUMBER_OF_KERNEL_SECTORS = $$((($(KERNEL_C_BYTES_SIZE)+0x1FF)/0x200)) # Get the number of sectors that the kernel takes up.

HARSHFS_BYTES_SIZE = $$(wc -c < 'build_os/harshfs_kernel_initial_image.bin') # Get how many bytes the kernel takes up
NUMBER_OF_HARSHFS_SECTORS = $$((($(HARSHFS_BYTES_SIZE)+0x1FF)/0x200)) # Get the number of sectors that the kernel takes up.

OVERALL_BYTES_SIZE = $$(( $(KERNEL_C_BYTES_SIZE) + $(HARSHFS_BYTES_SIZE) ))
OVERALL_NUMBER_OF_SECTORS = $$(( $(NUMBER_OF_KERNEL_SECTORS) + $(NUMBER_OF_HARSHFS_SECTORS) ))

SECONDARY_BOOTSECTOR_SIZE = $$(wc -c < 'build_os/second_stage_bootsector.bin') # Get how many bytes the secondary bootsector takes up
NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS = $$((($(SECONDARY_BOOTSECTOR_SIZE)+0x1FF)/0x200)) # Get the number of sectors that the secondary bootsector takes up.
################################################################################################################################################################

# First rule is run by default
os.iso: kernel_initrd_run build_os/boot_sector_main.bin build_os/second_stage_bootsector.bin kernel.bin kernel.elf
	echo "Secondary Bootsector Size: ${SECONDARY_BOOTSECTOR_SIZE}"
	echo "Number of Secondary Bootsector Sectors: ${NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS}"
	echo "Kernel Size: ${KERNEL_C_BYTES_SIZE}"
	echo "Number of Kernel Sectors: ${NUMBER_OF_KERNEL_SECTORS}"
	echo "Kernel + Initrd Size: ${OVERALL_BYTES_SIZE}"
	echo "Number of Kernel + Initrd  Sectors: ${OVERALL_NUMBER_OF_SECTORS}"
	cat build_os/boot_sector_main.bin build_os/second_stage_bootsector.bin build_os/kernel.bin build_os/harshfs_kernel_initial_image.bin > ./build_os/os.iso


# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: build_os/start_kernel.o build_os/interrupt.o ${OBJ}
	${LD} ${LDFLAGS} -o ./build_os/kernel.bin $^ --oformat binary

# Used for debugging purposes
kernel.elf: build_os/start_kernel.o build_os/interrupt.o ${OBJ}
	${LD} ${LDFLAGS} -o ./build_os/kernel.elf $^

run: os.iso
	qemu-system-i386 -fda build_os/os.iso

run-curses: os.iso
	qemu-system-i386 -curses -fda build_os/os.iso

# Open the connection to qemu and load our kernel-object file with symbols
debug: os.iso kernel.elf
	qemu-system-i386 -s -S -fda format=raw ./build_os/os.iso &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ./build_os/kernel.elf"

# Open the connection to qemu and load our kernel-object file with symbols
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

kernel_initrd:
	/usr/bin/gcc -g initrd/create_kernel_initrd.c -o build_os/create_kernel_initrd
	chmod +x build_os/create_kernel_initrd

kernel_initrd_run: kernel_initrd
	valgrind --leak-check=full --show-leak-kinds=all build_os/create_kernel_initrd

kernel_initrd_debug: kernel_initrd
	gdbserver :1234 /src/HarshOS/build_os/create_kernel_initrd

$(BUILD_DIR)/second_stage_bootsector.bin:
	nasm -f bin on_boot/second_stage_bootsector.s -o build_os/second_stage_bootsector.bin

$(BUILD_DIR)/boot_sector_main.bin: kernel.bin build_os/second_stage_bootsector.bin
	nasm -f bin -dNUMBER_OF_KERNEL_SECTORS=$(OVERALL_NUMBER_OF_SECTORS) -dKERNEL_C_BYTES_SIZE=$(OVERALL_BYTES_SIZE) -dSECONDARY_BOOTSECTOR_SIZE=$(SECONDARY_BOOTSECTOR_SIZE) -dNUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS=$(NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS) on_boot/boot_sector_main.s -o $@

clean:
	rm -rf build_os/*.bin build_os/*.dis build_os/*.o build_os/os.iso build_os/*.elf
	rm -rf build_os/*.o build_os/*.bin build_os/*.o
	rm -rf build_os/*.*
	rm -f build_os/create_kernel_initrd

github-actions-init:
	sudo apt-get update
	sudo apt-get remove docker docker-engine docker.io
	sudo apt install docker.io
	sudo systemctl start docker
	sudo systemctl enable docker
	docker --version
	docker pull sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest

github-actions-os-iso:
	docker run --rm --name harshos-github-actions docker pull sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest make os.iso

github-actions-kernel-elf:
	docker run --rm --name harshos-github-actions docker pull sriharshasingam/ubuntu-i386-elf-gcc-cross-compiler:latest make kernel.elf