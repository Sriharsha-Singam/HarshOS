//
// Created by Harsh on 6/3/20.
//

#include "page_frame_handler.h"
#include "../drivers/screen_control.h"
#include "kernel_malloc.h"


u32 get_frame_state_index_phys_addr(u32 physical_address) {
    u32 frame_number = physical_address >> 12;
    return (frame_number/32);
}
u32 get_frame_state_offse_phys_addr(u32 physical_address) {
    u32 frame_number = physical_address >> 12;
    return (frame_number%32);
}

u32 get_page_frame_free_list_state_address(u32 page_physical_address) {
    u32 state = frame_free_list[get_frame_state_index_phys_addr(page_physical_address)];
    u32 offset = 0x1 << get_frame_state_offse_phys_addr(page_physical_address);
    state &= offset;
    state = state >> get_frame_state_offse_phys_addr(page_physical_address);
    return state;
}

u32 get_page_frame_free_list_state_index_offset(u32 index, u32 offset) {
    u32 state = frame_free_list[index];
    state &= offset;
    state = state >> offset;
    return state;
}

u32 find_next_free_physical_page_frame() {
    for (u32 i = 0; i < number_of_indices_for_frame_free_list; i++) {
        if (frame_free_list[i] != 0xFFFFFFFF) {
            u32 offset = 0x1;
            for (int j = 0; j < 32; j++) {
                u32 index = frame_free_list[i];
                index &= offset;
                index = index >> i;
                if (index == 0) {
                    return ((i*32) + j);
                }
                offset = offset << 1;
            }
        }
    }
    return -1;
}

void set_page_frame_state(u32 page_physical_address, u32 state) {
    state = state << get_frame_state_offse_phys_addr(page_physical_address);
    u32 current_state = frame_free_list[get_frame_state_index_phys_addr(page_physical_address)];
    current_state |= state;
}

/**
 * Set A Page Directories Page Table
 * TODO: Add a User Mode for this =====>>> Using USER_CALLOC instead of kernel_calloc
 * TODO: SO FOR NOW I WILL CALL THIS FUNCTION -- KERNEL-SPECIFIC
 *
 * @return int 0=>{Changing Existing Page Table Setting} 1=>{Creating New Page Table} -1=>{FAILURE}
 */
u32 kernel_set_page_directory_entry(page_directory_t* page_directory, u32 page_dir_index, u8 is_user_mode, u8 is_writeable) {

    if (page_directory->page_directory_entries[page_dir_index].present) {
        page_directory->page_directory_entries[page_dir_index].user_or_kernel_mode = is_user_mode;
        page_directory->page_directory_entries[page_dir_index].read_or_write = is_writeable;
        return 0;
    } else {
        u32 new_page_table_physical = 0;
        page_table_t *new_page_table = (page_table_t *) kernel_calloc_page_aligned(sizeof(page_table_t), 0, &new_page_table_physical);

        new_page_table_physical = new_page_table_physical >> 12;

        LOG_DEBUG("New Kernel Page Table: ", (u32)new_page_table);

        page_directory->page_directory_entries[page_dir_index].user_or_kernel_mode = is_writeable;
        page_directory->page_directory_entries[page_dir_index].read_or_write = is_user_mode;
        page_directory->page_directory_entries[page_dir_index].page_table_address = new_page_table_physical;
        page_directory->page_directory_entries[page_dir_index].present = 1;

        return (u32)new_page_table;
    }

    return -1;
}

/**
 * This function can set/get a Page Table or a Page Directory Entry.
 *
 * @param page_directory
 * @param physical_address
 * @param is_user_mode      SET PAGE DIRECTORY TO USER MODE
 * @param is_writeable      SET PAGE DIRECTORY TO WRITEABLE
 * @param only_get          ONLY GET THE PAGE TABLE (IF IT EXISTS, THEN ONLY GET IT AND NOT CHANGE THE SETTINGS)
 *
 * @return THIS FUNCTION RETURNS THE PHYSICAL ADDRESS WHERE IS THE PAGE TABLE ARRAY IS STORED. THE ADDRESS IS NOT THE
 *          ADDRESS FOR ANY USER/KERNEL DATA. IT IS WHERE PAGE TABLE ENTRIES ARE STORED  -- EASILY CONFUSABLE
 */
u32 kernel_set_page_table(page_directory_t* page_directory, u32 physical_address, u8 is_user_mode, u8 is_writeable, u8 only_get) {



    u32 page_frame_number = physical_address >> 12;

    u32 page_table_physical = 0;

    if (!(page_directory->page_directory_entries[page_frame_number].present)) {

//        page_table_t *new_page_table = (page_table_t *)
        kernel_calloc_page_aligned(sizeof(page_table_t), 0, &page_table_physical);
        page_table_physical = page_table_physical >> 12;

        page_directory->page_directory_entries[page_frame_number].page_table_address = page_table_physical;

        page_directory->page_directory_entries[page_frame_number].user_or_kernel_mode = is_user_mode;
        page_directory->page_directory_entries[page_frame_number].read_or_write = is_writeable;

        page_table_physical = page_table_physical << 12;

        return page_table_physical;
    }

    page_table_physical = page_directory->page_directory_entries[page_frame_number].page_table_address;
    page_table_physical = page_table_physical << 12;

    if (only_get) return page_table_physical;

    page_directory->page_directory_entries[page_frame_number].user_or_kernel_mode = is_user_mode;
    page_directory->page_directory_entries[page_frame_number].read_or_write = is_writeable;

    return page_table_physical;
}

void kernel_set_page_table_entry(page_table_t* page_table, u32 physical_address, u32 page_table_entry_counter, u8 is_user_mode, u8 is_writeable) {

    u32 frame_number = physical_address >> 12;

    if (!(page_table->page_table[page_table_entry_counter].present)) {
        page_table->page_table[page_table_entry_counter].page_frame_address = frame_number;
    }
    page_table->page_table[page_table_entry_counter].read_or_write = is_writeable;
    page_table->page_table[page_table_entry_counter].user_or_kernel_mode = is_user_mode;
    page_table->page_table[page_table_entry_counter].present = 1;


    page_table->page_table[page_table_entry_counter].page_frame_address = frame_number;

    set_page_frame_state(physical_address, PRESENT);

}

void kernel_set_any_page_table_entry(page_table_t* page_table, u32 page_table_index, u8 is_user_mode, u8 is_writeable) {
    u32 physical_page_frame_number = find_next_free_physical_page_frame();
    kernel_set_page_table_entry(page_table, (physical_page_frame_number << 12), page_table_index, is_user_mode, is_writeable);
}

void kernel_free_page_table_entry(page_entry_t* page_entry) {

    if(page_entry->present || page_entry->page_frame_address) {

        u32 frame_physical_address = page_entry->page_frame_address;
        frame_physical_address = frame_physical_address << 12;

        set_page_frame_state(frame_physical_address, NOT_PRESENT);

        page_entry->present = 0;
        page_entry->page_frame_address = 0;

    }
}