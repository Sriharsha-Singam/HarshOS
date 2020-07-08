//
// Created by Sriharsha Singam on 6/30/2020.
//

#define HARSHFS 0x1

#include "harshfs.h"
#include "../memory/mem_operations.h"
#include "../memory/kernel_malloc.h"
#include "../memory/kernel_heap.h"
#include "../helper/string.h"
#include "../drivers/screen_control.h"
#include "../memory/page_frame_handler.h"

extern u32 KERNEL_END_VIRTUAL_ADDRESS;
extern u32 KERNEL_VIRTUAL_MAPPED_BASE;
u32 kernel_code_final_address = (u32)&KERNEL_END_VIRTUAL_ADDRESS;
u32 starting_virtual_address = (u32)&KERNEL_VIRTUAL_MAPPED_BASE;

char *errors[] = {
        "FILE_NAME_EXISTS",
        "FILE_PARENT_DIR_NOT_FOUND",
        "UNKNOWN_FILE_TYPE",
        "DATA_INPUT_IS_INVALID"
};

u32 filesystem_address = 0xF0000000;
u32 final_file_address = 0x0;
u32 overall_fs_size = 0;

harshfs_node* ROOT_NODE = NULL;


u32 get_header_or_footer(u8* data) {
    u32 header_or_footer = 0;

    for (u8 i = 0; i < 4; i++) {
        header_or_footer = header_or_footer << 8;
        header_or_footer |= *data;
        data++;
    }

    return header_or_footer;
}

int check_validity(u8* data) {

    u32 header = get_header_or_footer(data);
    data += 4;

    if (header != ALL_FILES_HEADER_MAGIC_NUMBER) return -1;

    u32 size = 0;
    while(get_header_or_footer(data) != ALL_FILES_FOOTER_MAGIC_NUMBER) {
        if (size >= 0xFFFFFFFF) return -1;
        data++;
        size++;
    }
    return 0;
}

u32 get_data_size(u8* data) {

    if (check_validity(data) == -1) return 0;
    data += 4;

    u32 size = 0;
    while(get_header_or_footer(data) != ALL_FILES_FOOTER_MAGIC_NUMBER) {
        if (size >= 0xFFFFFFFF) return 0;
        data++;
        size++;
    }
    return size;
}

harshfs_node* get_filesystem_root() {
    return ROOT_NODE;
}

harshfs_node* read_harshfs_kernel_image() {

    kernel_print_string("READING HARSHFS KERNEL IMAGE\n");
    kernel_print_string("Here are the files\n");

    u8* get_file = (u8*)filesystem_address;

    get_file += 4;

    u32 latest_size = 0;
    while (get_header_or_footer(get_file) != FILESYSTEM_FOOTER_MAGIC_NUMBER) {

        if (get_header_or_footer(get_file) == ALL_FILES_HEADER_MAGIC_NUMBER) {
            return ROOT_NODE;
        } else {
            harshfs_node* node = (harshfs_node*) get_file;
            if (latest_size == 0) ROOT_NODE = node;
            latest_size += node->size+8;
            kernel_print_string(node->name);
            kernel_print_string("\n");
            get_file += sizeof(harshfs_node);
        }
    }

    return ROOT_NODE;
}

void load_harshfs_kernel_image() {

    u32 page_table_entry_counter = 0;
    u32 physical_address = filesystem_address;
    u32 initial_physical_address = filesystem_address;
    page_table_t* new_page_table = (page_table_t*)kernel_set_page_directory_entry(get_current_page_directory(), KERNEL_PAGE_HARSHFS_NUMBER, KERNEL_MODE, WRITEABLE);
    while (physical_address <= (0x400000+initial_physical_address)) {

        kernel_set_page_table_entry(new_page_table, physical_address, page_table_entry_counter, KERNEL_MODE, WRITEABLE);

        set_page_frame_state(physical_address, PRESENT);

        page_table_entry_counter++;
        physical_address += 0x1000;
    }

    kernel_print_hex_value(kernel_code_final_address);

    kernel_print_string("Starting to Copy the Initial Kernel Image to this Address: 0xF0000000\n");

    u8* kernel_final_address = (u8*)kernel_code_final_address;

    while (get_header_or_footer(kernel_final_address) != FILESYSTEM_HEADER_MAGIC_NUMBER) {
        kernel_final_address++;
    }

    if (get_header_or_footer(kernel_final_address) == FILESYSTEM_HEADER_MAGIC_NUMBER) {

        kernel_print_string("HarshFS Filesystem Starting Address: ");
        kernel_print_hex_value((u32)kernel_final_address);
        kernel_print_string("\n");

        u32 kernel_filesystem_size = 0;
        u32 address_of_filesystem_start = (u32)kernel_final_address;
//        u32 physical_address = kernel_final_address - starting_virtual_address;
//        physical_address = physical_address & 0xFFFFF000;
//        u32 initial_physical_address = physical_address;

        while (get_header_or_footer(kernel_final_address) != FILESYSTEM_FOOTER_MAGIC_NUMBER) {
            kernel_filesystem_size++;
            kernel_final_address++;
        }

        kernel_filesystem_size += 4;

        kernel_print_string("HarshFS Filesystem Size: ");
        kernel_print_hex_value((u32)kernel_filesystem_size);
        kernel_print_string("\n");

//        memory_copy((char*)address_of_filesystem_start, (char*)filesystem_address, kernel_filesystem_size);





        memory_copy((char*)address_of_filesystem_start, (char*)filesystem_address, kernel_filesystem_size);
    }

}




























//u32 filesystem_address = 0xF0000000;
//
//harshfs_node* ROOT_NODE = NULL;
//
////void add_file_directory() {
////    filesystem_address +=
////}
//
//harshfs_node* insert_harshfs_node(harshfs_node* parent_directory, u8 file_type, char name[], u8* data, u8* error_code) {
//
//    if (parent_directory == NULL) {
//        if (ROOT_NODE == NULL) {
//            ROOT_NODE = kernel_heap_calloc(sizeof(harshfs_node), 0, NULL);
//            ROOT_NODE->harshfs_node_type = DIRECTORY;
//            for (int i = 0; i < string_length(name); i++) {
//                if (i == 256) break;
//                ROOT_NODE->name[i] = name[i];
//            }
//            ROOT_NODE->previous_dir = NULL;
//            ROOT_NODE->previous_in_current_dir = NULL;
//            ROOT_NODE->next_dir = NULL;
//            ROOT_NODE->next_in_current_dir = NULL;
//            ROOT_NODE->data = NULL;
//            ROOT_NODE->size = 0;
//
////            filesystem_address +=
//
//            return ROOT_NODE;
//        } else {
//            *error_code = FILE_PARENT_DIR_NOT_FOUND;
//            return NULL;
//        }
//    }
//
//    if (file_type == DIRECTORY) {
//        harshfs_node* new_directory = kernel_heap_calloc(sizeof(harshfs_node), 0, NULL);
//        if (parent_directory->next_dir == NULL) {
//            parent_directory->next_dir = new_directory;
//        } else {
//            harshfs_node* directory = parent_directory->next_dir;
//            while (directory) {
//                if (!string_compare(directory->name, name)) {
//                    *error_code = FILE_NAME_EXISTS;
//                    kernel_heap_free(new_directory);
//                    return NULL;
//                }
//                if (directory->next_in_current_dir == NULL) break;
//                directory = directory->next_in_current_dir;
//            }
//            directory->next_in_current_dir = new_directory;
//            new_directory->previous_in_current_dir = directory;
//            new_directory->next_in_current_dir = NULL;
//            new_directory->next_dir = NULL;
//            new_directory->previous_dir = parent_directory;
//            new_directory->size = 0;
//            new_directory->harshfs_node_type = DIRECTORY;
//            for (int i = 0; i < string_length(name); i++) {
//                if (i == 256) break;
//                new_directory->name[i] = name[i];
//            }
//            return new_directory;
//        }
//    }
//
//    if (file_type == FILE) {
//        harshfs_node* new_node = kernel_heap_calloc(sizeof(harshfs_node), 0, NULL);
//        harshfs_node* parent_copy = parent_directory;
//
//        while(parent_copy->next_in_current_dir) {
//            parent_copy = parent_copy->next_in_current_dir;
//        }
//        parent_copy->next_in_current_dir = new_node;
//        new_node->previous_in_current_dir = parent_copy;
//        new_node->next_in_current_dir = NULL;
//        new_node->next_dir = NULL;
//        new_node->previous_dir = NULL;
//        new_node->harshfs_node_type = FILE;
//        new_node->size = 0;
//        for (int i = 0; i < string_length(name); i++) {
//            if (i == 256) break;
//            new_node->name[i] = name[i];
//        }
//        new_node->data = NULL;
//
//        return new_node;
//    }
//
//    *error_code = UNKNOWN_FILE_TYPE;
//    return NULL;
//}
//
////void remove_harshfs_node(harshfs_node* remove) {
////
////}
//
//void add_file(harshfs_node* parent_directory, char name[], u8* error_code, void* data) {
//    filesystem_address += sizeof(harshfs_node);
//    insert_harshfs_node(parent_directory, FILE, name, data, error_code);
//}
//
//void add_directory(harshfs_node* parent_directory, char name[], u8* error_code) {
//    filesystem_address += sizeof(harshfs_node);
//    insert_harshfs_node(parent_directory, DIRECTORY, name, NULL, error_code);
//}
//
