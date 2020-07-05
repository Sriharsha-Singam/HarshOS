//
// Created by INT_ACC on 7/1/2020.
//

/**
 * THIS FILE WILL BE USED TO CREATE THE HARSH_FILESYSTEM BINARY -- SHOULD NOT BE CROSS-COMPILED
 *      --  This file will generate a custom kernel initrd binary file from certain parameters
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "create_kernel_initrd.h"
#include "../memory/mem_operations.h"
#include "../helper/string.h"

u32 filesystem_address = 0xF0000000;

harshfs_node* ROOT_NODE = NULL;
harshfs_node_address* ROOT_NODE_ADDRESS = NULL;

u32 overall_fs_size = 0;

void add_node_address(harshfs_node* node) {
    harshfs_node_address* new_node_address = (harshfs_node_address*) malloc(sizeof(harshfs_node_address));
    memset(new_node_address, 0, sizeof(harshfs_node_address));
    new_node_address->node = node;
    new_node_address->address = filesystem_address;
    new_node_address->next = NULL;

    if (ROOT_NODE_ADDRESS != NULL) {
        harshfs_node_address* nodes = ROOT_NODE_ADDRESS;
        while (nodes->next) nodes = nodes->next;
        nodes->next = new_node_address;
    } else {
        ROOT_NODE_ADDRESS = new_node_address;
    }
}

u32 get_node_address(harshfs_node* node) {

    if (ROOT_NODE_ADDRESS == NULL || node == NULL) return NULL;

    harshfs_node_address* nodes = ROOT_NODE_ADDRESS;
    while (nodes) {
        if (nodes->node == node) return nodes->address;
        nodes = nodes->next;
    }

    return 0;
}

harshfs_node* get_real_node_address_index(u32 index) {

    if (ROOT_NODE_ADDRESS == NULL) return NULL;

    harshfs_node_address* nodes = ROOT_NODE_ADDRESS;
    while (nodes->next && index) {
        nodes = nodes->next;
        index--;
    }

    return nodes->node;
}

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

u8* create_data(u8* data, u32 size) {

    u8* correct_data = (u8*)malloc((size+8));
    memset(correct_data, 0, size+8);
    u8* initial_pointer = correct_data;

    u32 header = ALL_FILES_HEADER_MAGIC_NUMBER;
    u32 footer = ALL_FILES_FOOTER_MAGIC_NUMBER;

    for (u32 i = 0; i < 4; i++) {
        *correct_data = ((header >> (24-8*i)) & 0x000000FF);
        correct_data++;
    }

    for (u32 i = 0; i < size; i++) {
        *correct_data = *data;
        correct_data++;
        data++;
    }

    for (u32 i = 0; i < 4; i++) {
        *correct_data = ((footer >> (24-8*i)) & 0x000000FF);
        correct_data++;
    }

    return initial_pointer;
}

void print_all_data(u8* data, u32 size) {
    printf("HEADER => 0x");
    for (u32 i = 0; i < size; i++) {
        printf("%1x", (*(data++)));
        if (i == (4-1)) printf("\nDATA   => 0x");
        if (i == (size-5)) printf("\nFOOTER => 0x");
    }
    printf("\n");
}

void print_data(u8* data, u32 size) {
    u8* data_copy = data + 4;
    for (u32 i = 0; i < size; i++) {
        printf("%1x", (*(data_copy++)));
    }
    printf(" => \"");
    data_copy = data + 4;
    for (u32 i = 0; i < size; i++) {
        printf("%c", (*(data_copy++)));
    }
    printf("\"\n");
}

int set_node(harshfs_node* node, char name[], u8 harshfs_node_type, harshfs_node* next_in_current_dir, harshfs_node* previous_in_current_dir, harshfs_node* next_dir, harshfs_node* previous_dir, u8* data) {
    for (u32 i = 0; i < strlen(name); i++) {
        if (i == 256) break;
        node->name[i] = name[i];
    }
    node->harshfs_node_type = harshfs_node_type;
    node->previous_dir = previous_dir;
    node->previous_in_current_dir = previous_in_current_dir;
    node->next_dir = next_dir;
    node->next_in_current_dir = next_in_current_dir;

    if (data != NULL) {
        if (check_validity(data) == -1) {
            free(node);
            return -1;
        }
        node->data = data;
        node->size = get_data_size(data);
    } else {
        node->data = 0;
        node->size = 0;
    }

    overall_fs_size += node->size;

    return 0;
}

harshfs_node* insert_harshfs_node(harshfs_node* parent_directory, u8 file_type, char name[], u8* data, u8* error_code) {

    if (parent_directory == NULL) {
        if (ROOT_NODE == NULL) {
            ROOT_NODE = malloc(sizeof(harshfs_node));
            memset(ROOT_NODE, 0, sizeof(harshfs_node));
            if (set_node(ROOT_NODE, name, DIRECTORY, NULL, NULL, NULL, NULL, NULL) == -1) {
                *error_code = DATA_INPUT_IS_INVALID;
                return NULL;
            }
            return ROOT_NODE;
        } else {
            *error_code = FILE_PARENT_DIR_NOT_FOUND;
            return NULL;
        }
    }

    if (file_type == DIRECTORY || file_type == FILE) {
        harshfs_node* new_directory = malloc(sizeof(harshfs_node));
        memset(new_directory, 0, sizeof(harshfs_node));
        harshfs_node* previous_in_current_dir = NULL;

        if (parent_directory->next_dir == NULL) {
            if (!strcmp(parent_directory->name, name)) {
                *error_code = FILE_NAME_EXISTS;
                free(new_directory);
                return NULL;
            }
            parent_directory->next_dir = new_directory;
            previous_in_current_dir = NULL;
        } else {
            harshfs_node* directory = parent_directory->next_dir;
            while (directory) {
                if (!strcmp(directory->name, name)) {
                    *error_code = FILE_NAME_EXISTS;
                    free(new_directory);
                    return NULL;
                }
                if (directory->next_in_current_dir == NULL) break;
                directory = directory->next_in_current_dir;
            }
            directory->next_in_current_dir = new_directory;
            previous_in_current_dir = directory;
        }

        if(set_node(new_directory, name, file_type, NULL, previous_in_current_dir, NULL, parent_directory, data) == -1) {
            free(new_directory);
            *error_code = DATA_INPUT_IS_INVALID;
            return NULL;
        }

        return new_directory;
    }

    *error_code = UNKNOWN_FILE_TYPE;
    return NULL;
}

harshfs_node* add_file(harshfs_node* parent_directory, char name[], u8* error_code, u8* data) {
    filesystem_address += sizeof(harshfs_node);
    overall_fs_size += sizeof(harshfs_node);
    harshfs_node* new_node = insert_harshfs_node(parent_directory, FILE, name, data, error_code);
    if (*error_code) return NULL;
    add_node_address(new_node);
    return new_node;
}

harshfs_node* add_directory(harshfs_node* parent_directory, char name[], u8* error_code) {
    filesystem_address += sizeof(harshfs_node);
    overall_fs_size += sizeof(harshfs_node);
    harshfs_node* new_node = insert_harshfs_node(parent_directory, DIRECTORY, name, NULL, error_code);
    if (*error_code) return NULL;
    add_node_address(new_node);
    return new_node;
}

u8 is_error(u8 error_code) {

    if (error_code) {
        printf("ERROR: %s\n", errors[--error_code]);
        printf("------------------------------------------------------------------------------\n");
        return 1;
    }
    return 0;
}

void print_node(harshfs_node* node, u8 error_code) {

    if (is_error(error_code)) return;

    char file[15] = "FILE\0";
    char directory[15] = "DIRECTORY\0";

    printf("------------------------------------------------------------------------------\n");
    printf("HARSHFS_NODE LISTING:\n  "
           "- NAME: %s\n  "
           "- SIZE: %i\n  "
           "- NODE_TYPE: %s\n  "
           "- NEXT_CURRENT_DIR: %s\n  "
           "- PREVIOUS_CURRENT_DIR: %s\n  "
           "- NEXT_DIR: %s\n  "
           "- PREVIOUS_DIR: %s\n  "
           "- DATA: ",
           node->name,
           node->size,
           ((node->harshfs_node_type == FILE) ? file : directory),
           (node->next_in_current_dir != NULL) ? node->next_in_current_dir->name : NULL,
           (node->previous_in_current_dir != NULL) ? node->previous_in_current_dir->name : NULL,
           (node->next_dir != NULL) ? node->next_dir->name : NULL,
           (node->previous_dir != NULL) ? node->previous_dir->name : NULL);

    if (node->harshfs_node_type == FILE) {
        u32* data_copy = (u32*) node->data;
        if (data_copy != NULL) {
            print_data((u8*)data_copy, (node->size));
        } else {
            printf("NULL\n");
        }
    } else {
        printf("NULL\n");
    }
    printf("------------------------------------------------------------------------------\n");
}

void add_4bytes_to_char_pointer(u8* addr, u32 value) {
    u8* values = (u8*) &value;
    values += 3;
    for (u32 i = 0; i < 4; i++) {
        *addr = *values;
        addr++;
        values--;
    }
}

void creating_image() {
    u8* image = (u8*) malloc((overall_fs_size));
    u8* image_copy = image;
    memset(image, 0, (overall_fs_size));

    add_4bytes_to_char_pointer(image, FILESYSTEM_HEADER_MAGIC_NUMBER);
    printf("%4x\n\n", FILESYSTEM_HEADER_MAGIC_NUMBER);
    image += 4;

    harshfs_node_address* node_address = ROOT_NODE_ADDRESS;
    while (node_address) {
        harshfs_node* node = node_address->node;
        u32 node_addr = get_node_address(node);

        u32 next_in_current_dir = get_node_address(node->next_in_current_dir);
        u32 previous_in_current_dir = get_node_address(node->previous_in_current_dir);
        u32 next_dir = get_node_address(node->next_dir);
        u32 previous_dir = get_node_address(node->previous_dir);

        harshfs_node* real_node = (harshfs_node*) malloc(sizeof(harshfs_node));
        memset(real_node, 0, sizeof(harshfs_node));

//        real_node->name = node->name;
        memcpy(real_node->name, node->name, 256);
        real_node->harshfs_node_type = node->harshfs_node_type;
        real_node->size = node->size;
        real_node->next_in_current_dir = (harshfs_node*)next_in_current_dir;
        real_node->previous_in_current_dir = (harshfs_node*)previous_in_current_dir;
        real_node->next_dir = (harshfs_node*)next_dir;
        real_node->previous_dir = (harshfs_node*)previous_dir;

        if (node->data && node->size) {
            real_node->data = (u8*)filesystem_address;
//            for (u32 i = 0; i < (node->size+8); i++) {
//                *(image + i + (filesystem_address - 0xF0000000)) = *(node->data + i);
//            }
            memcpy((image+(filesystem_address - 0xF0000000)), node->data, (node->size+8));
            filesystem_address += node->size+8;
        } else {
            real_node->data = NULL;
        }

        memcpy(image, real_node, sizeof(harshfs_node));
        harshfs_node* image_copy_struct = image;
        printf("%s\n", image_copy_struct->size);
//        print_node(image_copy_struct, NULL);
        image += sizeof(harshfs_node);
//        for (u32 i = 0; i < sizeof(harshfs_node); i++) {
//            *image = (u8)*real_node;
//            real_node++;
//            image++;
//        }

//        u8* extra = (u8*) malloc(sizeof(harshfs_node));
//        memcpy(extra, real_node, sizeof(harshfs_node));
//
//        for (u32 i = 0; i < sizeof(harshfs_node); i++) {
//            printf("%1x", *(extra + i));
//        }
//        printf("\n\n");

        free(real_node);

        node_address = node_address->next;
    }

    add_4bytes_to_char_pointer(image, FILESYSTEM_FOOTER_MAGIC_NUMBER);
    printf("%4x", FILESYSTEM_FOOTER_MAGIC_NUMBER);
    image += 4;

    printf("\n\n");

    for (u32 i = 0; i < overall_fs_size; i++) {
        printf("%1x", (*(image_copy + i)));
    }
    printf("\n\n");
    for (u32 i = 0; i < overall_fs_size; i++) {
        printf("%c", (*(image_copy + i)));
    }
}

int main() {

//    u8* get = (char*) malloc(4, 0);
//    u8* get = (char*) malloc(4, 0);
//    u32 get1 = 0xDC523775;
//    printf("Using this value: %4x\n", get1);
//    add_4bytes_to_char_pointer(get, get1);
//
//    printf("Testing Function: 0x");
//    for (u32 i = 0; i < 4; i++) {
//        printf("%1x", *get);
//        get++;
//    }
//    printf("\n");
//    get -= 4;
//    printf("%4x\n", get_header_or_footer(get));
//
//    if (get1 == get_header_or_footer(get)) printf("EQUAL\n");

    printf("\n\n|------------------------------------------------------------------------------|\n");
    printf("*******************Creating Kernel HarshFS Initial Filesystem*******************\n");

    filesystem_address += 4;
    overall_fs_size += 4;

    u8 error_code = 0;

    harshfs_node* node;

    node = add_directory(NULL, "harsh_root", &error_code);
    print_node(ROOT_NODE, error_code);

    node = add_file(ROOT_NODE, "test.txt", &error_code, NULL);
    print_node(node, error_code);

    node = add_directory
            (ROOT_NODE, "dir1", &error_code);
    print_node(node, error_code);

    harshfs_node* node_copy = node;
    node = add_file(node_copy, "test_dir1.txt\0", &error_code, create_data((u8*)"Hello World", 11));
    print_node(node, error_code);

    node = add_file(node, "test_dir1.txt\0", &error_code, NULL);
    print_node(node, error_code);

    filesystem_address += 4;
    overall_fs_size += 4;

    printf("*************FINISHED - Creating Kernel HarshFS Initial Filesystem**************\n");
    printf("|------------------------------------------------------------------------------|\n");

    creating_image();
//    harshfs_node* node_get = get_real_node_address_index(0);
//    print_node(node_get, NULL);
//
//    node_get = get_real_node_address_index(1);
//    print_node(node_get, NULL);

    return 0;
}