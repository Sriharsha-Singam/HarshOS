%ifdef BIN
[org 0x7c00] ; bootloader offset
%endif

SECONDARY_BOOTLOADER_ADDRESS equ 0x1000
KERNEL_PHYSICAL_ADDRESS equ 0x9000
KERNEL_VIRTUAL_ADDRESS equ 0xc0009000
TEMP_STACK_SIZE equ 0x100

harshos_start:
mov [BOOT_DRIVE], dl

; Setting up Boot Stack using the TEMP_STACK memory location. The stack moves from higher addresses
; to lower addresses.
mov bp, 0
add bp, TEMP_STACK_SIZE
add bp, TEMP_STACK
mov sp, bp

call load_bootloader_and_kernel
;jmp $
call switch_to_32bit

;%include "./on_boot/boot_sector_asm_files/boot_sector_print_16bit.s"
%include "./on_boot/boot_sector_asm_files/boot_sector_disk.s"
%include "./on_boot/boot_sector_asm_files/gdt.s"
%include "./on_boot/boot_sector_asm_files/convert_to_32bit_mode.s"

[bits 32]
START_32_BITS:
    call SECONDARY_BOOTLOADER_ADDRESS
    ;jmp $
    ;call KERNEL_PHYSICAL_ADDRESS

    mov eax, gdt_descriptor ; Push gdt descriptor virtual address
    add eax, 0xc0000000
    push eax

    ;mov eax, 0x3000
    ;mov dword [eax], 0
    ;invlpg [0]

    mov ecx, 0xc0009000
    ;jmp 1b
    ;cli
    ;hlt
    jmp ecx
    jmp $

; data
BOOT_DRIVE db 0

TEMP_STACK:
    times TEMP_STACK_SIZE db 0

; padding and magic number
times  510-($-$$) db 0
dw 0xaa55
