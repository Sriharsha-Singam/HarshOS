//
// Created by Harsh on 6/3/20.
//

#include "page_frame_handler.h"

#include "../drivers/screen_control.h"

u32 get_frame_state_index(u32 frame_number) {
    return (frame_number/32);
}
u32 get_frame_state_offset(u32 frame_number) {
    return (frame_number%32);
}

static void set_frame_address_state(u32 page_frame_address, u32* frame_state) {
    u32 frame_number = page_frame_address/0x1000;
    frame_state[get_frame_state_index(frame_number)] |= (0x1 << get_frame_state_offset(frame_number));
}

static int find_new_frame(u32* frame_state) {

    for (u32 i = 0; i < number_of_indices_for_frame_state; i++) {
        if (frame_state[i] != 0xFFFFFFFF) {
            for (u32 j = 0; j < 32; j++) {
                u32 single_frame_state = frame_state[i];
                single_frame_state = single_frame_state & (0x1 << j);

                if (single_frame_state == 0x0) {
                    return ((i * 32) + j);
                }
            }
        }
    }
    return -1;
}

void initialize_new_frame(page_entry_t* frame, u8 is_kernel, u8 is_writeable, u32* frame_state) {

    if (frame->page_frame_address) return;
    else {
        int new_frame_index = find_new_frame(frame_state);

        if (new_frame_index < 0) {
            kernel_print_string("PANIC: NO MORE PAGE FRAMES AVAILABLE. NOT ENOUGH SPACE. SHUTTING CPU DOWN\n");
            kernel_print_string("Shutting Down the CPU. Good Bye :(");
            asm volatile("hlt");
        }

        set_frame_address_state((new_frame_index * 0x1000), frame_state);

        frame->present = 1;
        frame-> read_or_write = is_writeable;
        frame->user_or_kernel_mode = !is_kernel;
        frame->page_frame_address = (new_frame_index * 0x1000);
    }
}