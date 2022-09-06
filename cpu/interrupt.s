[extern interrupt_service_request_handler] ; function from interrupt_handler.c
[extern interrupt_request_pic_handler] ; function from interrupt_handler.c

set_data_segments:
    mov ds, ax
   	mov es, ax
   	mov fs, ax
   	mov gs, ax
   	ret

interrupt_service_hander_caller:
    pusha
    mov ax, ds
    push eax
    mov ax, 16 ; This is data segment of the kernel as described in the GDT
    call set_data_segments
    call interrupt_service_request_handler
    pop eax
    call set_data_segments
    popa
    add esp, 8
    ;cli
    iret

interrupt_request_pic_hander_caller:
    pusha
    mov ax, ds
    push eax
    mov ax, 16 ; This is data segment of the kernel as described in the GDT
    call set_data_segments
    call interrupt_request_pic_handler
    pop eax
    call set_data_segments
    popa
    add esp, 8
    ;sti
    iret

global interrupt_service_request0
global interrupt_service_request1
global interrupt_service_request2
global interrupt_service_request3
global interrupt_service_request4
global interrupt_service_request5
global interrupt_service_request6
global interrupt_service_request7
global interrupt_service_request8
global interrupt_service_request9
global interrupt_service_request10
global interrupt_service_request11
global interrupt_service_request12
global interrupt_service_request13
global interrupt_service_request14
global interrupt_service_request15
global interrupt_service_request16
global interrupt_service_request17
global interrupt_service_request18
global interrupt_service_request19
global interrupt_service_request20
global interrupt_service_request21
global interrupt_service_request22
global interrupt_service_request23
global interrupt_service_request24
global interrupt_service_request25
global interrupt_service_request26
global interrupt_service_request27
global interrupt_service_request28
global interrupt_service_request29
global interrupt_service_request30
global interrupt_service_request31
global interrupt_service_request128

global interrupt_request_pic0
global interrupt_request_pic1
global interrupt_request_pic2
global interrupt_request_pic3
global interrupt_request_pic4
global interrupt_request_pic5
global interrupt_request_pic6
global interrupt_request_pic7
global interrupt_request_pic8
global interrupt_request_pic9
global interrupt_request_pic10
global interrupt_request_pic11
global interrupt_request_pic12
global interrupt_request_pic13
global interrupt_request_pic14
global interrupt_request_pic15



; 0: Divide By Zero Exception
interrupt_service_request0:
    cli
    push byte 0
    push byte 0
    jmp interrupt_service_hander_caller

; 1: Debug Exception
interrupt_service_request1:
    cli
    push byte 0
    push byte 1
    jmp interrupt_service_hander_caller

; 2: Non Maskable Interrupt Exception
interrupt_service_request2:
    cli
    push byte 0
    push byte 2
    jmp interrupt_service_hander_caller

; 3: Int 3 Exception
interrupt_service_request3:
    cli
    push byte 0
    push byte 3
    jmp interrupt_service_hander_caller

; 4: INTO Exception
interrupt_service_request4:
    cli
    push byte 0
    push byte 4
    jmp interrupt_service_hander_caller

; 5: Out of Bounds Exception
interrupt_service_request5:
    cli
    push byte 0
    push byte 5
    jmp interrupt_service_hander_caller

; 6: Invalid Opcode Exception
interrupt_service_request6:
    cli
    push byte 0
    push byte 6
    jmp interrupt_service_hander_caller

; 7: Coprocessor Not Available Exception
interrupt_service_request7:
    cli
    push byte 0
    push byte 7
    jmp interrupt_service_hander_caller

; 8: Double Fault Exception (With Error Code!)
interrupt_service_request8:
    cli
    push byte 8
    jmp interrupt_service_hander_caller

; 9: Coprocessor Segment Overrun Exception
interrupt_service_request9:
    cli
    push byte 0
    push byte 9
    jmp interrupt_service_hander_caller

; 10: Bad TSS Exception (With Error Code!)
interrupt_service_request10:
    cli
    push byte 10
    jmp interrupt_service_hander_caller

; 11: Segment Not Present Exception (With Error Code!)
interrupt_service_request11:
    cli
    push byte 11
    jmp interrupt_service_hander_caller

; 12: Stack Fault Exception (With Error Code!)
interrupt_service_request12:
    cli
    push byte 12
    jmp interrupt_service_hander_caller

; 13: General Protection Fault Exception (With Error Code!)
interrupt_service_request13:
    cli
    push byte 13
    jmp interrupt_service_hander_caller

; 14: Page Fault Exception (With Error Code!)
interrupt_service_request14:
    cli
    push byte 14
    jmp interrupt_service_hander_caller

; 15: Reserved Exception
interrupt_service_request15:
    cli
    push byte 0
    push byte 15
    jmp interrupt_service_hander_caller

; 16: Floating Point Exception
interrupt_service_request16:
    cli
    push byte 0
    push byte 16
    jmp interrupt_service_hander_caller

; 17: Alignment Check Exception
interrupt_service_request17:
    cli
    push byte 0
    push byte 17
    jmp interrupt_service_hander_caller

; 18: Machine Check Exception
interrupt_service_request18:
    cli
    push byte 0
    push byte 18
    jmp interrupt_service_hander_caller

; 19: Reserved
interrupt_service_request19:
    cli
    push byte 0
    push byte 19
    jmp interrupt_service_hander_caller

; 20: Reserved
interrupt_service_request20:
    cli
    push byte 0
    push byte 20
    jmp interrupt_service_hander_caller

; 21: Reserved
interrupt_service_request21:
    cli
    push byte 0
    push byte 21
    jmp interrupt_service_hander_caller

; 22: Reserved
interrupt_service_request22:
    cli
    push byte 0
    push byte 22
    jmp interrupt_service_hander_caller

; 23: Reserved
interrupt_service_request23:
    cli
    push byte 0
    push byte 23
    jmp interrupt_service_hander_caller

; 24: Reserved
interrupt_service_request24:
    cli
    push byte 0
    push byte 24
    jmp interrupt_service_hander_caller

; 25: Reserved
interrupt_service_request25:
    cli
    push byte 0
    push byte 25
    jmp interrupt_service_hander_caller

; 26: Reserved
interrupt_service_request26:
    cli
    push byte 0
    push byte 26
    jmp interrupt_service_hander_caller

; 27: Reserved
interrupt_service_request27:
    cli
    push byte 0
    push byte 27
    jmp interrupt_service_hander_caller

; 28: Reserved
interrupt_service_request28:
    cli
    push byte 0
    push byte 28
    jmp interrupt_service_hander_caller

; 29: Reserved
interrupt_service_request29:
    cli
    push byte 0
    push byte 29
    jmp interrupt_service_hander_caller

; 30: Reserved
interrupt_service_request30:
    cli
    push byte 0
    push byte 30
    jmp interrupt_service_hander_caller

; 31: Reserved
interrupt_service_request31:
    cli
    push byte 0
    push byte 31
    jmp interrupt_service_hander_caller

; 128: Reserved
interrupt_service_request128:
    cli
    push 0
    push 128
    jmp interrupt_service_hander_caller


; IRQ handlers
interrupt_request_pic0:
	cli
	push byte 0
	push byte 32
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic1:
	cli
	push byte 1
	push byte 33
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic2:
	cli
	push byte 2
	push byte 34
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic3:
	cli
	push byte 3
	push byte 35
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic4:
	cli
	push byte 4
	push byte 36
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic5:
	cli
	push byte 5
	push byte 37
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic6:
	cli
	push byte 6
	push byte 38
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic7:
	cli
	push byte 7
	push byte 39
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic8:
	cli
	push byte 8
	push byte 40
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic9:
	cli
	push byte 9
	push byte 41
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic10:
	cli
	push byte 10
	push byte 42
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic11:
	cli
	push byte 11
	push byte 43
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic12:
	cli
	push byte 12
	push byte 44
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic13:
	cli
	push byte 13
	push byte 45
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic14:
	cli
	push byte 14
	push byte 46
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic15:
	cli
	push byte 15
	push byte 47
	jmp interrupt_request_pic_hander_caller

custom_func:
    mov eax, 0
    int 0x80
