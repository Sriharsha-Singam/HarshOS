//
// Created by Harsh on 5/26/20.
//

#ifndef OS_32_PAGING_H
#define OS_32_PAGING_H

#include "../cpu/types.h"

/**
*   --------------------           ----------------                 --------------------
*   | Page_Directory * |  ---->    | Page_Table * |      ---->      | Page_Table_Entry |
*   --------------------           ----------------                 --------------------
*                                  | Page_Table * |                           .
*                                  ----------------                           .      1024th Tables Entries
*                                         .                                   .
*                        1024th Tables    .                         --------------------
*                                         .                         | Page_Table_Entry |
*                                  ----------------                 --------------------
*                                  | Page_Table * |
*                                  ----------------
*
*   THIS DATA STRUCTURE TAKES UP 4MB OUT OF 4GB -- 4GB IS THE MAXIMUM POSSIBLE ADDRESSABLE RANGE IN 32-BIT
*   ARCHITECTURE.
*
*/
typedef struct {
    u32 present: 1; /** Set if the page is present in memory. */
    u32 read_or_write: 1; /** If set, that page is writeable. If unset, the page is read-only. */
    u32 user_or_kernel_mode: 1; /** If set, this is a user-mode page. Else it is a supervisor (kernel)-mode page.
                                 * User-mode code cannot write to or read from kernel-mode pages. */
    u32 reserved: 2; /** Cannot be edited. This can only be used by CPU*/
    u32 accessed: 1; /** Get's checked by CPU when accesssed */
    u32 dirty: 1; /** Set when the page is written to. This means it will eventually need to be written back to the Hard Drive */
    u32 UNUSED: 3; /** --- UNUSED --- */
    u32 page_frame_address: 20; /** This is the HIGH 20 bits of the physical address for the Page Frame.
                                 *  The lower 20-bits will always be 0 sinze the frame size used is 4kB.*/
} __attribute__((packed)) page_entry_t;

typedef struct {
    /**
     * Each Page Table contains 1024 Page Entries that map to Physical Addresses
     */
    page_entry_t page_table[1024];
} __attribute__((packed)) page_table_t;

typedef struct {

    /**
     * This is an Array with Page Table Pointers
     * with all the Page Table Entries.
     *  -- This allows for virtual-to-physical page translations
     */
    page_table_t *virtual_page_table_addresses[1024];

    /**
     * This is an array of the physical address corresponding
     * to the virtual table addresses above
     *
     * Also this is the pointer used to set the CR3 register.
     */
    u32 physical_page_table_addresses[1024];

//    u32 physical_address_page_table;

} __attribute__((packed)) page_directory_t;


/**
 * Run all necessary commands in order to get paging to work and
 * to inform the CPU/Registers that paging is enabled.
 * This will allow the MMU to start PAGING and VIRTUALIZATION
 */
void start_paging();

#endif //OS_32_PAGING_H