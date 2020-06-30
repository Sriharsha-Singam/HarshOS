//
// Created by Sriharsha Singam on 5/26/20.
//

#ifndef HARSHOS_PAGING_H
#define HARSHOS_PAGING_H

#include "../cpu/types.h"


// The size of physical memory. For the moment we assume it is 16MB big.
#define TOTAL_RAM_MB_SIZE 0x1
#define final_physical_address (TOTAL_RAM_MB_SIZE * 0x1000000)
#define number_of_possible_page_frames (final_physical_address / 0x1000)  // Divinding Final Possible Address with the size of one page (0x1000 = 4096B)to get the number of total possible frames
#define number_of_bytes_for_frame_free_list (number_of_possible_page_frames/8)
#define number_of_indices_for_frame_free_list (number_of_possible_page_frames/32)

#define KERNEL_VIRTUAL_ADDRESS 0xc0000000
#define KERNEL_PAGE_NUMBER (KERNEL_VIRTUAL_ADDRESS >> 22)

extern u32* frame_free_list;

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

/** Based on -- https://www.cs.miami.edu/home/burt/learning/Csc521.131/docs/64-ia-32-architectures-software-developer-vol-3a-part-1-manual-p129.pdf0*/

typedef struct {
    u32 present: 1; /** Set if the page is present in memory. */
    u32 read_or_write: 1; /** If set, that page is writeable. If unset, the page is read-only. */
    u32 user_or_kernel_mode: 1; /** If set, this is a user-mode page. Else it is a supervisor (kernel)-mode page.
                                 * User-mode code cannot write to or read from kernel-mode pages. */
    u32 reserved: 2; /** Cannot be edited. This can only be used by CPU*/
    u32 accessed: 1; /** Get's checked by CPU when accesssed */
    u32 dirty: 1; /** Set when the page is written to. This means it will eventually need to be written back to the Hard Drive */
    u32 reserved1: 2;
    u32 UNUSED: 3; /** --- UNUSED --- */
    u32 page_frame_address: 20; /** This is the HIGH 20 bits of the physical address for the Page Frame.
                                 *  The lower 20-bits will always be 0 since the frame size used is 4kB.*/
} page_entry_t;

typedef struct {
    /**
     * Each Page Table contains 1024 Page Entries that map to Physical Addresses
     */
    page_entry_t page_table[1024];
} page_table_t;

typedef struct {
    u32 present: 1; /** Set if the page is present in memory. */
    u32 read_or_write: 1; /** If set, that page is writeable. If unset, the page is read-only. */
    u32 user_or_kernel_mode: 1; /** If set, this is a user-mode page. Else it is a supervisor (kernel)-mode page.
                                 * User-mode code cannot write to or read from kernel-mode pages. */
    u32 reserved: 2; /** Cannot be edited. This can only be used by CPU*/
    u32 accessed: 1; /** Get's checked by CPU when accesssed */
    u32 UNUSED0: 1; /** --- UNUSED --- */
    u32 ZERO: 1; /** HAS TO BE SET TO ZERO */
    u32 UNUSED1: 4; /** --- UNUSED --- */
    u32 page_table_address: 20; /** Address to the Page Table -- 20 bits relevant*/
} page_directory_entry_t;

typedef struct {

    page_directory_entry_t page_directory_entries[1024];
//    /**
//     * This is an Array with Page Table Pointers
//     * with all the Page Table Entries.
//     *  -- This allows for virtual-to-physical page translations
//     */
//    page_table_t *virtual_page_table_addresses[1024];
//
//    /**
//     * This is an array of the physical address corresponding
//     * to the virtual table addresses above
//     *
//     * Also this is the pointer used to set the CR3 register.
//     */
//    u32 physical_page_table_addresses[1024];
//
//    /**
//      * Memory address that contains bits that state whether
//      * a frame is being used already or not.
//      */
//    u32* frame_state;

//    u32 physical_address_page_table;

} page_directory_t;

/**
 * Run all necessary commands in order to get paging to work and
 * to inform the CPU/Registers that paging is enabled.
 * This will allow the MMU to start PAGING and VIRTUALIZATION
 */
void start_paging();
void setup_page_fault_interrupt_handler();
void wait();
page_directory_t* create_new_page_directory(u8 is_kernel_page_directory);
void set_active_page_directory(page_directory_t* page_directory);
//static void page_fault_interrupt(interrupt_inputs_t input);
//void identity_page_memory_segment(page_entry_t* starting_page_table_entry, u32 starting_virtual_addresses, u32 memory_size, u8 is_kernel, u8 is_writeable);
//void initialize_new_page_directory();
//page_directory_t* create_new_page_directory();
//void change_active_page_directory(page_directory_t *dir);
//page_entry_t *get_page(u32 frame_address, u32 make, page_directory_t *page_directory);

#endif //HARSHOS_PAGING_H