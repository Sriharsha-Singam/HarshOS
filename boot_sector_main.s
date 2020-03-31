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

;mov bp, 0x8000 ; set the stack safely away from us
;mov sp, bp

;call printn

;mov ebx, BOOTING16
;call print


;call printn

;mov bx, 0x9000 ; es:bx = 0x0000:0x9000 = 0x09000
;mov dh, 2
;call disk_read


;mov dx, [0x9000] ; retrieve the first loaded word, 0xdada
;call print_hex

;call printn

;mov dx, [0x9000 + 512] ; first word from second loaded sector, 0xface
;call print_hex

;push dx
;mov dx, sp
;call print_hex
;pop dx

;mov dx, 0x12fe
;call print_hex

%include "./boot_sector_asm_files/boot_sector_print_16bit.s"
%include "./boot_sector_asm_files/boot_sector_disk.s"
%include "./boot_sector_asm_files/boot_sector_print.s"
%include "./boot_sector_asm_files/gdt.s"
%include "./boot_sector_asm_files/convert_to_32bit_mode.s"

[bits 32]
START_32_BITS:
;    mov ebx, SPACES
;    call print
;    mov ebx, BOOTING32
;    call print

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