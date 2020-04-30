C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)
BUILD_DIR = build_os
# Nice syntax for file extension replacement
OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SOURCES)))

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g

# First rule is run by default
os.bin: build_os/boot_sector_main.bin kernel.bin
	cat build_os/boot_sector_main.bin build_os/kernel.bin > ./build_os/os.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: build_os/start_kernel.o ${OBJ}
	i386-elf-ld -o ./build_os/kernel.bin -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: build_os/start_kernel.o ${OBJ}
	i386-elf-ld -o ./build_os/kernel.elf -Ttext 0x1000 $^

run: os.bin
	qemu-system-i386 -fda build_os/os.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: os.bin kernel.elf
	qemu-system-i386 -s -S -fda ./build_os/os.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ./build_os/kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
$(BUILD_DIR)/%.o: kernel/%.c
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: drivers/%.c drivers/%.h
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

$(BUILD_DIR)/%.o: on_boot/%.s
	nasm $< -f elf -o $@

$(BUILD_DIR)/%.bin: on_boot/%.s
	nasm $< -f bin -o $@

clean:
	rm -rf build_os/*.bin build_os/*.dis build_os/*.o build_os/os.bin build_os/*.elf
	rm -rf build_os/*.o build_os/*.bin build_os/*.o