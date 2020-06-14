[bits 32]

global SECOND_STAGE_BOOTLOADER_LABEL

SECOND_STAGE_BOOTLOADER_LABEL:
pusha
;mov ebp, 0
;add ebp, SECOND_TEMP_STACK_SIZE
;add ebp, SECOND_TEMP_STACK
;mov esp, ebp

mov edx, VIDEO_MEMORY
lea ebx, [SECONDARY_LOADING_KERNEL]
add ebx, 0x1000

call print
call SETUP_BOOT_KERNEL_PAGE_TABLE
call SETUP_BOOT_KERNEL_PAGE_DIRECTORY

mov eax, BOOT_PAGE_DIRECTORY_ABSOLUTE_ADDRESS

mov cr3, eax
mov eax, cr0 ; Enable paging (set PG bit in cr0 register)
or eax, 0x80000000
mov cr0, eax

;jmp $
popa
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SETUP_BOOT_KERNEL_PAGE_TABLE:
pusha
mov ebx, BOOT_KERNEL_PAGE_TABLE_ABSOLUTE_ADDRESS
mov eax, 0


FILL_BOOT_KERNEL_PAGE_TABLE:
cmp eax, 0x400000 ; 4MB Memory
jz BOOT_KERNEL_PAGE_TABLE_SETUP_DONE
or eax, 0x7
mov [ebx], eax
add ebx, 4
and eax, 0xFFFFF000
add eax, 0x1000
jmp FILL_BOOT_KERNEL_PAGE_TABLE

BOOT_KERNEL_PAGE_TABLE_SETUP_DONE:
popa
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SETUP_BOOT_KERNEL_PAGE_DIRECTORY:
pusha
mov ebx, BOOT_PAGE_DIRECTORY_ABSOLUTE_ADDRESS

mov eax, [ebx]
or eax, 0x7
mov [ebx], eax

add ebx, KERNEL_PAGE_NUMBER*4
mov eax, [ebx]
or eax, 0x7
mov [ebx], eax

BOOT_KERNEL_PAGE_DIRECTORY_SETUP_DONE:
popa
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%include "./on_boot/boot_sector_asm_files/boot_sector_print.s"

START_PAGING_TABLE:
    db "Starting to Create Kernel Boot Page Table", 0
END_PAGING_TABLE:
    db "Finished Creating Kernel Boot Page Table", 0
SECONDARY_LOADING_KERNEL:
    db "Secondary Bootloader has been Loaded and is now Running..."
BOOTING32:
    db "                      Booting -- HarshOS -- in 32 bit protected mode...", 0

KERNEL_PHYSICAL_ADDRESS equ 0x9000
KERNEL_VIRTUAL_ADDRESS equ 0xc0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_ADDRESS >> 22)

BOOT_KERNEL_PAGE_TABLE_ABSOLUTE_ADDRESS equ (BOOT_KERNEL_PAGE_TABLE + 0x1000)
BOOT_PAGE_DIRECTORY_ABSOLUTE_ADDRESS equ (BOOT_PAGE_DIRECTORY + 0x1000)

;SECOND_TEMP_STACK_SIZE equ 0x500
;SECOND_TEMP_STACK:
;   times SECOND_TEMP_STACK_SIZE db 0

align 0x1000
BOOT_KERNEL_PAGE_TABLE:
    times 1024 dd 0  ; Page Table has 1024 32-bit entries

align 0x1000
BOOT_PAGE_DIRECTORY:
    dd BOOT_KERNEL_PAGE_TABLE_ABSOLUTE_ADDRESS           ; This Page Table contains the First MB of Memory which contains the Kernel -- This will be used for Identity Mapping the Kernel and other important memory mapped address like 0xB8000 for VGA
    times (KERNEL_PAGE_NUMBER - 1) dd 0 ; Other Page Directory Entries must be set to 0
    dd BOOT_KERNEL_PAGE_TABLE_ABSOLUTE_ADDRESS           ; This Page Table contains the First MB of Memory which contains the Kernel -- This will be used for Virtually Addressing the Kernel
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0 ; Other Page Directory Entries must be set to 0

times 512-(($-$$) % 512) db 0
