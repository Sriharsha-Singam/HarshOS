[bits 32] ; using 32-bit protected mode

; this is how constants are defined
;VIDEO_MEMORY equ  0xb8000
VIDEO_MEMORY equ  0xb86e0
WHITE_ON_BLACK equ 0x0F ; the color byte for each character


print:
    pusha
    mov edx, VIDEO_MEMORY

print_loop:
    mov al, [ebx] ; [ebx] is the address of our character
    mov ah, 0x0F

    cmp al, 0 ; check if end of string
    jz exit

    mov [edx], ax ; store character + attribute in video memory
    add ebx, 1 ; next char
    add edx, 2 ; next video memory position

    jmp print_loop

exit:
    ;mov ptr VIDEO_MEMORY, edx
    popa
    ret