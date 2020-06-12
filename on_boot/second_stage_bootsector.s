[bits 32]

global SECOND_STAGE_BOOTLOADER_LABEL

SECOND_STAGE_BOOTLOADER_LABEL:
;mov ebp, 0
;add ebp, SECOND_TEMP_STACK_SIZE
;add ebp, SECOND_TEMP_STACK
;mov esp, ebp

mov edx, VIDEO_MEMORY
lea ebx, [SECONDARY_LOADING_KERNEL]
add ebx, 0x1000
call print

ret
;all 0x9000
;jmp $

%include "./on_boot/boot_sector_asm_files/boot_sector_print.s"


SECONDARY_LOADING_KERNEL:
    db "Secondary Bootloader has been Loaded and is now Running...", 0
BOOTING32:
    db 'Booting OS in 32 bit protected mode...', 0

KERNEL_PHYSICAL_ADDRESS equ 0x9000
SECOND_TEMP_STACK_SIZE equ 0x500
SECOND_TEMP_STACK:
   times SECOND_TEMP_STACK_SIZE db 0

times 512-(($-$$) % 512) db 0
