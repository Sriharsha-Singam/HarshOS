# [extern interrupt_service_request_handler] # function from interrupt_handler.c
# [extern interrupt_request_pic_handler] # function from interrupt_handler.c
.intel_syntax noprefix

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
    mov ax, 16 # This is data segment of the kernel as described in the GDT
    call set_data_segments
    call interrupt_service_request_handler
    pop eax
    call set_data_segments
    popa
    add esp, 8
    #cli
    iret

interrupt_request_pic_hander_caller:
    pusha
    mov ax, ds
    push eax
    mov ax, 16 # This is data segment of the kernel as described in the GDT
    call set_data_segments
    call interrupt_request_pic_handler
    pop eax
    call set_data_segments
    popa
    add esp, 8
    #sti
    iret

.globl interrupt_service_request0
.globl interrupt_service_request1
.globl interrupt_service_request2
.globl interrupt_service_request3
.globl interrupt_service_request4
.globl interrupt_service_request5
.globl interrupt_service_request6
.globl interrupt_service_request7
.globl interrupt_service_request8
.globl interrupt_service_request9
.globl interrupt_service_request10
.globl interrupt_service_request11
.globl interrupt_service_request12
.globl interrupt_service_request13
.globl interrupt_service_request14
.globl interrupt_service_request15
.globl interrupt_service_request16
.globl interrupt_service_request17
.globl interrupt_service_request18
.globl interrupt_service_request19
.globl interrupt_service_request20
.globl interrupt_service_request21
.globl interrupt_service_request22
.globl interrupt_service_request23
.globl interrupt_service_request24
.globl interrupt_service_request25
.globl interrupt_service_request26
.globl interrupt_service_request27
.globl interrupt_service_request28
.globl interrupt_service_request29
.globl interrupt_service_request30
.globl interrupt_service_request31

.globl interrupt_request_pic0
.globl interrupt_request_pic1
.globl interrupt_request_pic2
.globl interrupt_request_pic3
.globl interrupt_request_pic4
.globl interrupt_request_pic5
.globl interrupt_request_pic6
.globl interrupt_request_pic7
.globl interrupt_request_pic8
.globl interrupt_request_pic9
.globl interrupt_request_pic10
.globl interrupt_request_pic11
.globl interrupt_request_pic12
.globl interrupt_request_pic13
.globl interrupt_request_pic14
.globl interrupt_request_pic15


&define interrupt_service_creator1(comment, num, one)
# $num: $comment
interrupt_service_request$num:
    cli
    push $one
    jmp interrupt_service_hander_caller
&end

&define interrupt_service_creator2(comment, num, one, two)
# $num: $comment
interrupt_service_request$num:
    cli
    push $one
    push $two
    jmp interrupt_service_hander_caller
&end

&define interrupt_pic_creator2(comment, num, one, two)
# $num: $comment
interrupt_request_pic$num:
    cli
    push $one
    push $two
    jmp interrupt_request_pic_hander_caller
&end

interrupt_service_creator2(Divide By Zero Exception           , 0, 0, 0)
interrupt_service_creator2(Debug Exception                    , 1, 0, 1)
interrupt_service_creator2(Non Maskable Interrupt Exception   , 2, 0, 2)
interrupt_service_creator2(Int 3 Exception                    , 3, 0, 3)
interrupt_service_creator2(INTO  Exception                    , 4, 0, 4)
interrupt_service_creator2(Out of Bounds Exception            , 5, 0, 5)
interrupt_service_creator2(Invalid Opcode Exception           , 6, 0, 6)
interrupt_service_creator1(Coprocessor Not Available Exception, 7, 0, 7)

interrupt_service_creator1(Double Fault Exception -- With Error Code!, 8, 8)

interrupt_service_creator2(Coprocessor Segment Overrun Exception, 9, 0, 9)

interrupt_service_creator1(Bad TSS Exception -- With Error Code!                 , 10, 10)
interrupt_service_creator1(Segment Not Present Exception -- With Error Code!     , 11, 11)
interrupt_service_creator1(Stack Fault Exception Exception -- With Error Code!   , 12, 12)
interrupt_service_creator1(General Protection Fault Exception -- With Error Code!, 13, 13)
interrupt_service_creator1(Page Fault Exception -- With Error Code!              , 14, 14)
interrupt_service_creator2(Reserved Exception -- With Error Code!                , 15, 0, 15)
interrupt_service_creator2(Floating Point Exception -- With Error Code!          , 16, 0, 16)
interrupt_service_creator2(Alignment CheckException -- With Error Code!          , 17, 0, 17)


# 18: Machine Check Exception
interrupt_service_request18:
    cli
    push 0
    push 18
    jmp interrupt_service_hander_caller

# 19: Reserved
interrupt_service_request19:
    cli
    push 0
    push 19
    jmp interrupt_service_hander_caller

# 20: Reserved
interrupt_service_request20:
    cli
    push 0
    push 20
    jmp interrupt_service_hander_caller

# 21: Reserved
interrupt_service_request21:
    cli
    push 0
    push 21
    jmp interrupt_service_hander_caller

# 22: Reserved
interrupt_service_request22:
    cli
    push 0
    push 22
    jmp interrupt_service_hander_caller

# 23: Reserved
interrupt_service_request23:
    cli
    push 0
    push 23
    jmp interrupt_service_hander_caller

# 24: Reserved
interrupt_service_request24:
    cli
    push 0
    push 24
    jmp interrupt_service_hander_caller

# 25: Reserved
interrupt_service_request25:
    cli
    push 0
    push 25
    jmp interrupt_service_hander_caller

# 26: Reserved
interrupt_service_request26:
    cli
    push 0
    push 26
    jmp interrupt_service_hander_caller

# 27: Reserved
interrupt_service_request27:
    cli
    push 0
    push 27
    jmp interrupt_service_hander_caller

# 28: Reserved
interrupt_service_request28:
    cli
    push 0
    push 28
    jmp interrupt_service_hander_caller

# 29: Reserved
interrupt_service_request29:
    cli
    push 0
    push 29
    jmp interrupt_service_hander_caller

# 30: Reserved
interrupt_service_request30:
    cli
    push 0
    push 30
    jmp interrupt_service_hander_caller

# 31: Reserved
interrupt_service_request31:
    cli
    push 0
    push 31
    jmp interrupt_service_hander_caller
    

# IRQ handlers
interrupt_request_pic0:
	cli
	push 0
	push 32
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic1:
	cli
	push 1
	push 33
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic2:
	cli
	push 2
	push 34
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic3:
	cli
	push 3
	push 35
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic4:
	cli
	push 4
	push 36
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic5:
	cli
	push 5
	push 37
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic6:
	cli
	push 6
	push 38
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic7:
	cli
	push 7
	push 39
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic8:
	cli
	push 8
	push 40
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic9:
	cli
	push 9
	push 41
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic10:
	cli
	push 10
	push 42
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic11:
	cli
	push 11
	push 43
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic12:
	cli
	push 12
	push 44
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic13:
	cli
	push 13
	push 45
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic14:
	cli
	push 14
	push 46
	jmp interrupt_request_pic_hander_caller

interrupt_request_pic15:
	cli
	push 15
	push 47
	jmp interrupt_request_pic_hander_caller
