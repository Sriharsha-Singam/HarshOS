;;;%include "boot_sector_disk_read.s"

;	AH = 02
;	AL = number of sectors to read	(1-128 dec.)
;	CH = track/cylinder number  (0-1023 dec., see below)
;	CL = sector number  (1-17 dec.)
;	DH = head number  (0-15 dec.)
;	DL = drive number (0=A:, 1=2nd floppy, 80h=drive 0, 81h=drive 1)
;	ES:BX = pointer to buffer


;	on return:
;	AH = status  (see INT 13,STATUS)
;	AL = number of sectors read
;	CF = 0 if successful
;	   = 1 if error


;	- BIOS disk reads should be retried at least three times and the
;	  controller should be reset upon error detection
;	- be sure ES:BX does not cross a 64K segment boundary or a
;	  DMA boundary error will occur
;	- many programming references list only floppy disk register values
;	- only the disk number is checked for validity
;	- the parameters in CX change depending on the number of cylinders;
;	  the track/cylinder number is a 10 bit value taken from the 2 high
;	  order bits of CL and the 8 bits in CH (low order 8 bits of track):

;	  |F|E|D|C|B|A|9|8|7|6|5-0|  CX
;	   | | | | | | | | | |	`-----	sector number
;	   | | | | | | | | `---------  high order 2 bits of track/cylinder
;	   `------------------------  low order 8 bits of track/cyl number
[bits 16]

disk_read:
    pusha
    push dx

    mov ah, 0x02 ;read

    mov ch, 0x00

    mov dh, 0x00
    mov dl, [BOOT_DRIVE]

    int 0x13

    ;jc disk_error
    pop dx
    ;cmp al, dh
    ;jne sector_error
    jmp disk_end

;disk_error:
;    mov bx, DISK_ERROR
;    call print16
;    call printn16
;    mov bx, 0
;    mov ah, 0x01
;    int 0x13
;    mov bl, al ; ah = error code, dl = disk drive that dropped the error
;    call print16_hex ; check out the code at http://stanislavs.org/helppc/int_13-1.html
;    jmp disk_end

;sector_error:
 ;   mov bx, SECTORS_ERROR
  ;  call print16

disk_end:
    popa
    ret

;DISK_ERROR: db "Disk read error", 0
;SECTORS_ERROR: db "Incorrect number of sectors read", 0