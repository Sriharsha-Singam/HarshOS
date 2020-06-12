[bits 16]
print16:
    pusha
print16_loop:
    mov al, [bx]
    cmp al, 0
    je exit16

    mov ah, 0x0E
    int 0x10
    add bx, 1
    jmp print16_loop

printn16:
    pusha
    mov ah, 0x0E
    mov al, 0x0A
    int 0x10

    mov al, 0x0D
    int 0x10
    jmp exit16

print16_hex:
    pusha
    mov cx, 0
    rol dx, 4

hex16_loop:
    cmp cx, 4
    je exit16

    mov ax, dx
    and ax, 0x000F
    add al, 0x30
    cmp al, 0x39
    jle print16_hex_digits
    add al, 7

print16_hex_digits:
    mov ah, 0x0E
    int 0x10
    rol dx, 4
    add cx, 1
    jmp hex16_loop


exit16:
    popa
    ret