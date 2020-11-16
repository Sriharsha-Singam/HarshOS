[bits 32]

;;;; A MAJOR REQUIREMENT FOR THIS CODE IS THAT ONLY ABSOLUTE ADDRESSES ARE USED AND NOT RELATIVE ONES -- SINCE PAGING IS GOING TO BE ENABLED.

; EAX contains the mallocable address

;START_PAGING:
;    extern end

;    mov eax, end
;    and eax, 0xFFF

;    cmp eax, 0x0
;    lea edi, [START_PAGING_DIRECTORY]
;    jz edi

;    and eax, 0xFFFFF000
;    add eax, 0x1000

;START_PAGING_DIRECTORY:  ; EAX currently contains the Page Directory starting address

;    mov edi, eax ; EDI is the PAGE DIRECTORY ADDRESS

    ; Need to get address for the FIRST PAGE TABLE location -- [1024*32 bits = 0x1000 bytes] after the PAGE DIRECTORY ADDRESS
;    add eax, 0x1000

;    mov esi, eax ; ESI is the FIRST PAGE TABLE ADDRESS
;    or esi, 0x7   ; Setting Present, R/W and User Mode -- For Now ==> TODO: Change R/W to only R and change User Mode to Kernel Mode
;    mov [edi], esi ; Placing the Page Table #0 Pointer along with Status Bits into the Page Directory
;    and esi, 0xFFFFF000 ; Setting ESI to only the end of the Page Table #0

   ; add eax, 0x300000 ; Getting Address for the 0x300th or the 768th PAGE TABLE ADDRESS
   ; or eax, 0x7 ; Setting Present, R/W and User Mode -- For Now ==> TODO: Change R/W to only R and change User Mode to Kernel Mode
   ; add edi, 0xC00 ; Getting the index address location of the #768 Page Directory Entry
   ; mov [edi], eax ; Placing the Page Table #768 Pointer along with Status Bits into the Page Directory
   ; and eax, 0xFFFFF000 ; Setting EAX to only the end of the Page Table #768


; FIRST 1MB needs to be Identity Mappped so that
;    mov ecx, 0
;    mov edx, esi
;IDENTITY_PAGE_1MB:

;    mov ebx, ecx ; Identity Address inside First 1MB
;    or ebx, 0x7  ; Setting Present, R/W and User Mode -- For Now ==> TODO: Change R/W to only R and change User Mode to Kernel Mode
;    mov [edx], ebx ; Setting Page Table Entry

;    add ecx, 0x1000 ; Increment to the next Page Address in the first 1MB -- increments of 0x1000
;    add edx, 0x4    ; Going to the next Page Table Entry Address in the Page Table

;    cmp ecx, 0x100000   ; Checking if the 1MB mark has been reached -- if so jump
;    lea edx, [ENABLE_PAGING]  ; Get the Absolute address for the jump location of the ENABLE_PAGING
;    jz edx  ; ENABLE PAGING NOW

;    lea ebx, [IDENTITY_PAGE_1MB]  ; Get the Absolute address for the jump location of the IDENTITY_PAGE_1MB
;    jmp ebx ; Next Page Frame to Identity Map


; PLACING Kernel Virtual Address at 0xC0000000
    ;mov ecx, 0
    ;mov edx, esi
    ;add edx, 0xC00
;MAP_HIGHER_HALF_KERNEL:

    ;mov ebx, ecx ; Identity Address inside First 1MB
    ;or ebx, 0x7  ; Setting Present, R/W and User Mode -- For Now ==> TODO: Change R/W to only R and change User Mode to Kernel Mode
    ;mov [edx], ebx ; Setting Page Table Entry

    ;add ecx, 0x1000 ; Increment to the next Page Address in the first 1MB -- increments of 0x1000
    ;add edx, 0x4    ; Going to the next Page Table Entry Address in the Page Table

    ;cmp ecx, 0x100000   ; Checking if the 1MB mark has been reached -- if so jump
    ;lea edx, [ENABLE_PAGING]  ; Get the Absolute address for the jump location of the ENABLE_PAGING
    ;jz edx  ; ENABLE PAGING NOW

    ;lea ebx, [IDENTITY_PAGE_1MB]  ; Get the Absolute address for the jump location of the IDENTITY_PAGE_1MB
    ;jmp ebx ; Next Page Frame to Identity Map

;ENABLE_PAGING:
;    mov cr3, edi
;    mov ebx, cr0        ; read current cr0
;    or  ebx, 0x80000000 ; set PG
;    mov cr0, ebx        ; update cr0

;    lea ebx, [START_32_BITS]
;    jmp ebx

;RETURN_FROM_PAGING:
;    ret
