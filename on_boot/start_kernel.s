[bits 32]
[extern KERNEL_VIRTUAL_MAPPED_BASE]
[extern kernel_main] ; Define calling point. Must have same name as kernel.c 'main' function

NEW_KERNEL_STACK_SIZE equ 0x4000

section .text

EDIT_GDT_STARTING_ADDRESS:
;jmp $
pop eax
mov ebx, [eax + 2]
add ebx, KERNEL_VIRTUAL_MAPPED_BASE
mov [eax + 2], ebx
lgdt [eax]

SETUP_NEW_KERNEL_STACK:
mov ebp, NEW_KERNEL_STACK_SIZE + NEW_KERNEL_STACK
mov esp, ebp

INVALIDATE_IDENTITY_MAPPED_KERNEL_REGION:
mov eax, cr3
mov ebx, 0
mov [eax], ebx
;jmp $
invlpg [0]

START_VIRTUAL_KERNEL:
mov eax, cr3
push eax
call kernel_main ; Calls the C function. The linker will know where it is placed in memory

;mov ebx, END_OF_KERNEL_RUN
;call print

jmp $

%include "./on_boot/boot_sector_asm_files/boot_sector_print.s"

section .data
END_OF_KERNEL_RUN: db "Kernel Main Has Ended. Waiting on Interrupts", 0

section .bss
align 0x4
NEW_KERNEL_STACK: resb NEW_KERNEL_STACK_SIZE