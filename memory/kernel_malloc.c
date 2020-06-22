//
// Created by INT_ACC on 6/6/2020.
//

#include "kernel_malloc.h"
#include "mem_operations.h"

/**
 * This END is a symbol made by using a custom linker script: link.ld.
 * This is the address that is located at the end of the kernel code.
 *   |--> Address to safely place any new variables/memory
 */
extern u32 KERNEL_END_VIRTUAL_ADDRESS;
extern u32 KERNEL_VIRTUAL_MAPPED_BASE;
/**
 * This local variable will act as the location pointer for any
 * future mallocs. We can safely use this address for malloc
 * knowing it is not going overwrite important code.
 */
u32 mallocable_address = (u32)&KERNEL_END_VIRTUAL_ADDRESS;
u32 kernel_virtual_base = (u32)&KERNEL_VIRTUAL_MAPPED_BASE;

u32 kernel_malloc(u32 mem_size, u32* physical_address) {

    // Storing the current free address to be able to return later
    u32 malloced_address = mallocable_address;

    /**
     * Adding the Memory Size to the local mallocable address.
     * This is calculating the next location to use for memory placement;
     */
    mallocable_address += mem_size;

    if (physical_address) {
        *physical_address = (malloced_address - kernel_virtual_base);
    }

    // returning the original mallocable_address
    return (malloced_address);

}

u32 kernel_malloc_page_aligned(u32 mem_size, u32* physical_address) {

    /**
     * Must check if the current mallocable_address is already page-aligned.
     * If it is not page-aligned it must be changed.
     *
     * The lower 12-bits of an address must be 0 for a page-aligned-starting
     * memory address. This is because each page is 4096 (0x1000) bytes
     * or 4kB -- so the starting address of every page must have 0s in the last 12-bits
     */
    if((mallocable_address & 0xFFF)) {
        mallocable_address = (mallocable_address & 0xFFFFF000) + 0x1000;
    }

    // Storing the current free address to be able to return later
    u32 malloced_address = mallocable_address;

    /**
    * Adding the Memory Size to the local mallocable address.
    * This is calculating the next location to use for memory placement;
    */
    mallocable_address += mem_size;

    if (physical_address) {
        *physical_address = (malloced_address - kernel_virtual_base);
    }

    // returning the original mallocable_address
    return (malloced_address);
}

u32 kernel_calloc(u32 mem_size, u8 value, u32* physical_address) {
    u32 malloced_address = kernel_malloc(mem_size, physical_address);
    memory_set((u32*)malloced_address, value, mem_size);
    return malloced_address;
}

u32 kernel_calloc_page_aligned(u32 mem_size, u8 value, u32* physical_address) {
    u32 malloced_address = kernel_malloc_page_aligned(mem_size, physical_address);
    memory_set((u32*)malloced_address, value, mem_size);
    return malloced_address;
}