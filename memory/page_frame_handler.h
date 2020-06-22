//
// Created by Harsh on 6/3/20.
//

#ifndef OS_32_PAGE_FRAMES_H
#define OS_32_PAGE_FRAMES_H

//#include "../cpu/types.h"
#include "paging.h"
#include "../kernel/defined_macros.h"

#define PRESENT 0x1
#define NOT_PRESENT 0x0

#define USER_MODE 1
#define KERNEL_MODE 0

#define READ_ONLY 0
#define WRITEABLE 1

u32 get_frame_state_index_phys_addr(u32 physical_address);
u32 get_frame_state_offse_phys_addr(u32 physical_address);

u32 get_page_frame_free_list_state_address(u32 page_physical_address);
u32 get_page_frame_free_list_state_index_offset(u32 index, u32 offset);

u32 find_next_free_physical_page_frame();
void set_page_frame_state(u32 page_physical_address, u32 state);

u32 kernel_set_page_directory_entry(page_directory_t* page_directory, u32 page_dir_index, u8 is_user_mode, u8 is_writeable);

u32 kernel_set_page_table(page_directory_t* page_directory, u32 physical_address, u8 is_user_mode, u8 is_writeable, u8 only_get);

void kernel_set_page_table_entry(page_table_t* page_table, u32 physical_address, u32 page_table_entry_counter, u8 is_user_mode, u8 is_writeable);
void kernel_set_any_page_table_entry(page_table_t* page_table, u32 page_table_index, u8 is_user_mode, u8 is_writeable);

#endif //OS_32_PAGE_FRAMES_H
