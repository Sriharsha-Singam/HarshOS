[org 0x7c00] ; bootloader offset
KERNEL_OFFSET equ 0x1000
mov [BOOT_DRIVE], dl

mov bp, 0x9000 ; set the stack
mov sp, bp

mov bx, BOOTING16
call print16
call printn16

call load_kernel
call switch_to_32bit

%include "./on_boot/boot_sector_asm_files/boot_sector_print_16bit.s"
%include "./on_boot/boot_sector_asm_files/boot_sector_disk.s"
%include "./on_boot/boot_sector_asm_files/boot_sector_print.s"
%include "./on_boot/boot_sector_asm_files/gdt.s"
%include "./on_boot/boot_sector_asm_files/convert_to_32bit_mode.s"

[bits 32]
START_32_BITS:
    call KERNEL_OFFSET
    jmp $


; data
BOOT_DRIVE db 0

BOOTING16:
    db 'Booting OS in 16 bit mode...', 0

BOOTING32:
    db 'Booting OS in 32 bit protected mode...', 0

LOADING_KERNEL:
    db 'Currently Loading Kernel...', 0

SPACES:
    db '                                                                                            ',0

; padding and magic number
times  510-($-$$) db 0
dw 0xaa55