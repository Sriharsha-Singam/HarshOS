//
// Created by Harsh on 6/3/20.
//

#include "paging.h"
#include "../cpu/interrupt_handler.h"
#include "page_frame_handler.h"
#include "kernel_malloc.h"
#include "mem_operations.h"
#include "../drivers/screen_control.h"
#include "../helper/string.h"

/**
 * Setup variables for Page Directories.
 * Each Process will use its own Page Directory.
 * So, One Page Directory is needed specially for the kernel process and,
 * one Page Directory variable that stores the pointer for the currently being used Page Directory
 *  -- which would correspond to the currently running process.
 */
page_directory_t* kernel_directory;
page_directory_t* currently_used_directory;

void wait() {
    for (int j = 0; j < 100000000 ; j++) {}
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
    kernel_print_string("} at 0x");
    kernel_print_hex_value(faulting_address);
    kernel_print_string("\n");
    //PANIC("Page fault");
}

void setup_page_fault_interrupt_handler() {
    set_interrupt_handler(14, page_fault_interrupt_handler);
}
//void testing_if_paging_enabled() {
//
//    u32 *ptr = (u32*)0xA0000000;
//    u32 do_page_fault = *ptr;
//
//    char value[11];
//    u32_to_hex_ascii(do_page_fault, value);
//
//    kernel_print_string("FINISHED Testing Paging :: ");
//    kernel_print_string(value);
//    kernel_print_string("\n");
//    wait();
//
//
//}

void start_paging() {


    kernel_print_string("Starting Paging\n");
    u32 val = sizeof(page_directory_t);
    u32 val1 = sizeof(page_table_t);
    char value[11];
    kernel_print_string("Paging Directory: ");
    u32_to_hex_ascii(val, value);
    kernel_print_string(value);
    kernel_print_string("\n");

    kernel_print_string("Paging Table: ");
    u32_to_hex_ascii(val1, value);
    kernel_print_string(value);
    kernel_print_string("\n");

    wait();
    kernel_directory = create_new_kernel_page_directory();
    kernel_print_string("Kernel Page Directory Created and Enabled in the registers\n");
    //set_interrupt_handler(14, page_fault_interrupt);
//    kernel_print_string("Testing if Paging is Enabled.\n");
//    testing_if_paging_enabled();
}

page_directory_t* create_new_kernel_page_directory() {

    page_directory_t* new_page_directory = (page_directory_t*)kernel_malloc_page_aligned(sizeof(page_directory_t));
    memory_set((u32*)new_page_directory, 0, sizeof(page_directory_t));

    page_table_t* new_page_table = (page_table_t*)kernel_malloc_page_aligned(sizeof(page_table_t));
    memory_set((u32*)new_page_directory, 0, sizeof(page_table_t));

    u32 new_page_table_address = (u32)new_page_table;
    new_page_table_address = new_page_table_address >> 12;

    new_page_directory->page_directory_entries[0].page_table_address = new_page_table_address;
    new_page_directory->page_directory_entries[0].ZERO = 0;
    new_page_directory->page_directory_entries[0].accessed = 0;
    new_page_directory->page_directory_entries[0].user_or_kernel_mode = 1;
    new_page_directory->page_directory_entries[0].read_or_write = 1;
    new_page_directory->page_directory_entries[0].present = 1;

    u32 physical_address = 0x0;
    u32 page_table_entry_counter = 0;
    while (physical_address < 0x100000) {

        new_page_table->page_table[page_table_entry_counter].present = 1;
        new_page_table->page_table[page_table_entry_counter].read_or_write = 1;
        new_page_table->page_table[page_table_entry_counter].user_or_kernel_mode = 1;

        u32 frame_number = physical_address >> 12;
        new_page_table->page_table[page_table_entry_counter].page_frame_address = frame_number;

        page_table_entry_counter++;
        physical_address += 0x1000;
    }

    set_active_page_directory(new_page_directory);

    return new_page_directory;
}

void set_active_page_directory(page_directory_t* page_directory) {

    currently_used_directory = page_directory;

//    u32 page_directory_address = page_directory->page_directory_entries[0].page_table_address
    asm volatile("mov %0, %%cr3":: "r"((u32)(page_directory->page_directory_entries)));
    u32 cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));

}


















//void testing_if_paging_enabled() {
//
//    u32 *ptr = (u32*)0xA0000000;
//    u32 do_page_fault = *ptr;
//
//    char value[11];
//    u32_to_hex_ascii(do_page_fault, value);
//
//    kernel_print_string("FINISHED Testing Paging :: ");
//    kernel_print_string(value);
//    kernel_print_string("\n");
//    wait();
//
//
//}
//
//void start_paging() {
//
//    kernel_print_string("Starting Paging\n");
//    wait();
//    kernel_directory = create_new_page_directory();
//    kernel_print_string("Kernel Page Directory Created and Enabled in the registers\n");
//    set_interrupt_handler(14, page_fault_interrupt);
//    kernel_print_string("Testing if Paging is Enabled.\n");
//    testing_if_paging_enabled();
//}
//
//
//void identity_page_memory_segment(page_entry_t* starting_page_table_entry, u32 starting_virtual_addresses, u32 memory_size, u8 is_kernel, u8 is_writeable) {
//
//    starting_virtual_addresses &= 0xFFFFF000;
//
//    while(memory_size > 0) {
//        starting_page_table_entry->page_frame_address = (starting_virtual_addresses >> 12);
//        starting_page_table_entry->user_or_kernel_mode = !is_kernel;
//        starting_page_table_entry->read_or_write = is_writeable;
//
//        starting_virtual_addresses += 0x1000;
//        memory_size -= 0x1000;
//        starting_page_table_entry++;
//    }
//
//}
//
//void initialize_new_page_directory() {
//
//}
//
//page_directory_t* create_new_page_directory() {
//
//
//    /**
//      * Getting the address for a memory space that contains the number
//      * of bytes necessary to store the state for each page frame
//      */
//    u32* frame_state = (u32*)kernel_malloc(number_of_bytes_for_frame_state);
//    memory_set(frame_state, 0, number_of_bytes_for_frame_state);
//
//    page_directory_t* page_directory = (page_directory_t*)kernel_malloc_page_aligned(sizeof(page_directory_t));
//    memory_set((u32*)page_directory, 0, sizeof(page_directory_t));
//    currently_used_directory = page_directory;
//
//    page_directory->frame_state = frame_state;
//
//    char value[11];
//    kernel_print_string("Paging Info: \nPage Directory Address: ");
//    u32_to_hex_ascii((u32)page_directory, value);
//    kernel_print_string(value);
//    kernel_print_string("\n");
//    wait();
////    kernel_print_string("Paging Info: ");
////    kernel_print_string("Paging Info: ");
////    kernel_print_string("Paging Info: ");
////    page_directory->
//
//
//
//
//    /**
//     * First thing first. The first 1MB of memory must be identity mapped to be able to access
//     * important address such as the VGA Pointer -- 0xB8000
//     *
//     * Identity Map: The virtual addresses and the physical addresses are the same.
//     */
//
//    u32 i = 0;
//    while (i < mallocable_address)
//    {
//        // Kernel code is readable but not writeable from userspace.
//        initialize_new_frame( get_page(i, 1, page_directory), 0, 0, frame_state);
//        i += 0x1000;
//    }
//
////    u32 one_mb = 0x100000;
////
////    u32 page_num = 0;
////    while (page_num < one_mb) {
////
////        identity_page_memory_segment();
////        page_num += 0x1000;
////    }
//
//    kernel_print_string("Kernel Page Directory has been Created and Set.\nGoing to enable paging in the Paging Registers\n");
//    wait();
//
//    change_active_page_directory(page_directory);
//
//    return page_directory;
//}
//
//void change_active_page_directory(page_directory_t *page_directory) {
//    currently_used_directory = page_directory;
//    asm volatile("mov %0, %%cr3":: "r"(&page_directory->physical_page_table_addresses));
//    u32 cr0;
//    asm volatile("mov %%cr0, %0": "=r"(cr0));
//    cr0 |= 0x80000000; // Enable paging!
//    asm volatile("mov %0, %%cr0":: "r"(cr0));
//}
//
//page_entry_t *get_page(u32 frame_address, u32 make, page_directory_t *page_directory) {
////    // Turn the address into an index.
////    frame_address /= 0x1000;
////    // Find the page table containing this address.
////    u32 table_idx = frame_address / 1024;
////    if (page_directory->virtual_page_table_addresses[table_idx]) // If this table is already assigned
////    {
////        return &page_directory->virtual_page_table_addresses[table_idx]->page_table[frame_address%1024];
////    }
////    else if(make)
////    {
////        u32 tmp;
////        page_directory->virtual_page_table_addresses[table_idx] = (page_table_t*)kernel_malloc_page_aligned(sizeof(page_table_t));
////        tmp = (u32)page_directory->virtual_page_table_addresses[table_idx];
////        memory_set((u32*)page_directory->virtual_page_table_addresses[table_idx], 0, 0x1000);
////        page_directory->physical_page_table_addresses[table_idx] = tmp | 0x7; // PRESENT, RW, US.
////        return &page_directory->virtual_page_table_addresses[table_idx]->page_table[frame_address%1024];
////    }
////    else
////    {
////        return 0;
////    }
//}
