//
// Created by Harsh on 6/3/20.
//

#ifndef OS_32_PAGE_FRAMES_H
#define OS_32_PAGE_FRAMES_H

#include "../cpu/types.h"
#include "paging.h"

u32 get_frame_state_index(u32 frame_number);
u32 get_frame_state_offset(u32 frame_number);
void initialize_new_frame(page_entry_t* frame, u8 is_kernel, u8 is_writeable, u32* frame_state);

#endif //OS_32_PAGE_FRAMES_H
