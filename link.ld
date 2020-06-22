ENTRY(kernel_main)

SECTIONS {

    . = 0xc0009000;
    KERNEL_VIRTUAL_ADDRESS = .;
    KERNEL_VIRTUAL_MAPPED_BASE = 0xc0000000;
    KERNEL_PHYSICAL_ADDRESS = KERNEL_VIRTUAL_ADDRESS - KERNEL_VIRTUAL_MAPPED_BASE;


    .text : AT(ADDR(.text) - KERNEL_VIRTUAL_MAPPED_BASE) {

        code = .;

        * (.text)
        . = ALIGN(0x1000);
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_VIRTUAL_MAPPED_BASE) {

         rodata = .;

         * (.rodata)
         . = ALIGN(0x1000);
    }

    .data : AT(ADDR(.data) - KERNEL_VIRTUAL_MAPPED_BASE) {

        data = .;

        * (.data)
        . = ALIGN(0x1000);
    }

    .bss : AT(ADDR(.bss) - KERNEL_VIRTUAL_MAPPED_BASE) {

        bss = .;

        * (.bss)
        . = ALIGN(0x1000);
    }

    KERNEL_END_VIRTUAL_ADDRESS = .;
    KERNEL_END_PHYSICAL_ADDRESS = KERNEL_END_VIRTUAL_ADDRESS - KERNEL_VIRTUAL_MAPPED_BASE;
}