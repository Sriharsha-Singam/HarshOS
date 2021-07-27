[bits 16]
load_bootloader_and_kernel:

    ; Load Secondary Bootloader from Boot Drive
    mov cl, 0x2
    mov bx, SECONDARY_BOOTLOADER_ADDRESS ; Read from disk and store in 0x1000
    mov al, NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS
    ;mov dl, [BOOT_DRIVE]
    call disk_read

    mov cl, 0x2
    add cl, NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS
    mov bx, 0x9000 ; KERNEL_PHYSICAL_ADDRESS ; Read from disk and store in 0x9000
    mov al, NUMBER_OF_KERNEL_SECTORS
    add al, NUMBER_OF_HARSHFS_SECTORS
    mov al, 56
    call disk_read

    ;mov cl, 0x2
    ;add cl, NUMBER_OF_SECONDARY_BOOTSECTOR_SECTORS
    ;add cl, 56 ; NUMBER_OF_KERNEL_SECTORS ; 56
    mov ch, 0x01
    mov cl, 0xb
    mov bx, 0x4000 ; Read from disk and store in 0x9000
    ;;mov al, NUMBER_OF_KERNEL_SECTORS
    mov al, 2  ;NUMBER_OF_HARSHFS_SECTORS
    call disk_read
    ;mov dx, 0
    ;mov dl, cl ; word [0x4200]
    ;mov byte [0x4000], 0x43
    ;call print16_hex
    ret

[bits 16]
switch_to_32bit:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_32bit

[bits 32]
init_32bit: ; we are now using 32-bit instructions
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    jmp START_32_BITS
    ;ret
