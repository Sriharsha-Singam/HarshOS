[org 0x7c00] ; bootloader offset

SECONDARY_BOOTLOADER_ADDRESS equ 0x1000
KERNEL_PHYSICAL_ADDRESS equ 0x9000
TEMP_STACK_SIZE equ 0x100


mov [BOOT_DRIVE], dl

; Setting up Boot Stack using the TEMP_STACK memory location. The stack moves from higher addresses
; to lower addresses.
mov bp, 0
add bp, TEMP_STACK_SIZE
add bp, TEMP_STACK
;mov bp, 0x9000
mov sp, bp

;mov bx, BOOTING16
;call print16
;call printn16

call load_bootloader_and_kernel
call switch_to_32bit

;call START_PAGING

;%include "./on_boot/boot_sector_asm_files/boot_sector_print_16bit.s"
%include "./on_boot/boot_sector_asm_files/boot_sector_disk.s"
;%include "./on_boot/boot_sector_asm_files/boot_sector_print.s"
%include "./on_boot/boot_sector_asm_files/gdt.s"
%include "./on_boot/boot_sector_asm_files/convert_to_32bit_mode.s"
;%include "./on_boot/paging.s"

[bits 32]
START_32_BITS:
  ;  mov ebx, BOOTING32
    ;call print
    ;jmp SECONDARY_BOOTLOADER_ADDRESS
    call 0x1000
    call 0x9000
    jmp $

; data
BOOT_DRIVE db 0
;KERNEL_OFFSET dd 0x10000

;BOOTING16:
;    db 'Booting OS in 16 bit mode...', 0

;BOOTING32:
 ;   db 'Booting OS in 32 bit protected mode...', 0

;LOADING_KERNEL:
;    db 'Hello World', 0

;SPACES:
 ;   db '                                                                                            ',0

TEMP_STACK:
    times TEMP_STACK_SIZE db 0

; padding and magic number
times  510-($-$$) db 0
dw 0xaa55
