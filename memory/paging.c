//
// Created by Sriharsha Singam on 6/3/20.
//

#include "page_frame_handler.h"
#include "../cpu/interrupt_handler.h"
#include "page_frame_handler.h"
#include "kernel_malloc.h"
#include "mem_operations.h"
#include "../drivers/screen_control.h"
#include "../helper/string.h"
#include "kernel_heap.h"

/**
 * Setup variables for Page Directories.
 * Each Process will use its own Page Directory.
 * So, One Page Directory is needed specially for the kernel process and,
 * one Page Directory variable that stores the pointer for the currently being used Page Directory
 *  -- which would correspond to the currently running process.
 */
page_directory_t* kernel_directory;
u32 boot_directory;
page_directory_t* currently_used_directory;
u32* frame_free_list = 0;

void wait() {
//    for (int j = 0; j < 100000000 ; j++) {}
}

static void page_fault_interrupt_handler(interrupt_inputs_t input) {
    if (!(input.interrupt_no)) return;

    kernel_print_string("PAGE FREAKING FAULT :: { ");
    u32 faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    u8 present = !(input.error_code & 0x1); // 1 if present
    u8 read_or_write = input.error_code & 0x2; // 1 if writeable
    u8 user_or_kernel_mode = input.error_code & 0x4; // 1 if user mode
    u8 reserved = input.error_code & 0x8; // Overwritten CPU-reserved bits of page entry?
    //u8 id = input.error_code & 0x10; // Caused by an instruction fetch?

    if (present) {
        kernel_print_string("page is present; ");
    } else {
        kernel_print_string("page is not present; ");
    }
    if (read_or_write) {
        kernel_print_string("address is writeable; ");
    } else {
        kernel_print_string("address is read-only; ");
    }
    if (user_or_kernel_mode) {
        kernel_print_string("cpu is user mode; ");
    } else {
        kernel_print_string("cpu is kernel mode; ");
    }
    if (reserved) {kernel_print_string("reserved ");}
    kernel_print_string("} at ");
    kernel_print_hex_value(faulting_address);
    kernel_print_string("; EIP:");
    kernel_print_hex_value(input.eip);
    kernel_print_string("\n");
    //PANIC("Page fault");
}

void start_paging() {
//    kernel_print_string("Starting Paging\n");

    set_interrupt_handler(14, page_fault_interrupt_handler);

    asm volatile("mov %%cr3, %0" : "=r"(boot_directory));

//    kernel_print_string("Boot Page Directoy (Used for booting): ");
//    kernel_print_hex_value((u32)boot_directory);
//    kernel_print_string("\n");

//    kernel_print_string("Paging Directory Size: ");
//    kernel_print_hex_value((u32)sizeof(page_directory_t));
//    kernel_print_string("\n");

//    kernel_print_string("Paging Table Size: ");
//    kernel_print_hex_value((u32)sizeof(page_table_t));
//    kernel_print_string("\n");

//    wait();
    frame_free_list = (u32*)kernel_heap_calloc(number_of_bytes_for_frame_free_list, 0, NULL);
    kernel_directory = create_new_page_directory(1);
//    wait();
//    kernel_print_string("Kernel Page Directory Created and Enabled in the registers\n");
}

page_directory_t* create_new_page_directory(u8 is_kernel_page_directory) {

    if (kernel_directory) is_kernel_page_directory = 0; // Prevent Bad use of this crucial Access Control Paging Function

    u32 new_page_directory_physical = 0;
    page_directory_t* new_page_directory = (page_directory_t*)kernel_heap_calloc_page_aligned(sizeof(page_directory_t), 0, &new_page_directory_physical);

    LOG_DEBUG("New Kernel Page Directory: ", (u32)new_page_directory);
//    LOG_DEBUG("New Kernel Page Directory -- Physical: ", (u32)new_page_directory_physical);
//    LOG_DEBUG("Page Directory Kernel Address: ", (u32)&(new_page_directory->page_directory_entries[KERNEL_PAGE_NUMBER]));

    page_table_t* new_page_table = 0;

    if (is_kernel_page_directory) {
        new_page_table = (page_table_t*)kernel_set_page_directory_entry(new_page_directory, KERNEL_PAGE_NUMBER, KERNEL_MODE, WRITEABLE);
    } else {
        new_page_table = (page_table_t*)kernel_set_page_directory_entry(new_page_directory, KERNEL_PAGE_NUMBER, USER_MODE, READ_ONLY);
    }

    u32 physical_address = 0x0;
    u32 page_table_entry_counter = 0;
    while (physical_address != 0x400000) {

        if (is_kernel_page_directory) {
            kernel_set_page_table_entry(new_page_table, physical_address, page_table_entry_counter, KERNEL_MODE, WRITEABLE);
        } else {
            kernel_set_page_table_entry(new_page_table, physical_address, page_table_entry_counter, USER_MODE, READ_ONLY);
        }

        set_page_frame_state(physical_address, PRESENT);

        page_table_entry_counter++;
        physical_address += 0x1000;
    }

    set_active_page_directory(new_page_directory);

    return new_page_directory;
}

void set_active_page_directory(page_directory_t* page_directory) {

    currently_used_directory = page_directory;

    u32 setting_page_directory = (u32)page_directory;
    setting_page_directory -= KERNEL_VIRTUAL_ADDRESS;

    asm volatile("mov %0, %%cr3":: "r"((u32)setting_page_directory));
}