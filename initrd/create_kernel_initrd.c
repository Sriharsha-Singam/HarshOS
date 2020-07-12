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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

char *errors[] = {
        "FILE_NAME_EXISTS",
        "FILE_PARENT_DIR_NOT_FOUND",
        "UNKNOWN_FILE_TYPE",
        "DATA_INPUT_IS_INVALID"
};

u32 filesystem_address = 0xF0000000;
u32 final_file_address = 0x0;

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

    final_file_address = filesystem_address;
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
        overall_fs_size += (node->size+8);
    } else {
        node->data = 0;
        node->size = 0;
    }

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

    if (file_type == DIRECTORY || file_type == FILE_CODE) {
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
    *error_code = NULL;
    harshfs_node* new_node = insert_harshfs_node(parent_directory, FILE_CODE, name, data, error_code);
    if (*error_code) return NULL;
    filesystem_address += sizeof(harshfs_node);
    overall_fs_size += sizeof(harshfs_node);
    add_node_address(new_node);
    return new_node;
}

harshfs_node* add_directory(harshfs_node* parent_directory, char name[], u8* error_code) {
    *error_code = NULL;
    harshfs_node* new_node = insert_harshfs_node(parent_directory, DIRECTORY, name, NULL, error_code);
    if (*error_code) return NULL;
    filesystem_address += sizeof(harshfs_node);
    overall_fs_size += sizeof(harshfs_node);
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
           ((node->harshfs_node_type == FILE_CODE) ? file : directory),
           (node->next_in_current_dir != NULL) ? node->next_in_current_dir->name : NULL,
           (node->previous_in_current_dir != NULL) ? node->previous_in_current_dir->name : NULL,
           (node->next_dir != NULL) ? node->next_dir->name : NULL,
           (node->previous_dir != NULL) ? node->previous_dir->name : NULL);

    if (node->harshfs_node_type == FILE_CODE) {
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

void print_node_binary(harshfs_node* image_copy_struct, u32 address, u8* address_of_data) {
    char file[15] = "FILE\0";
    char directory[15] = "DIRECTORY\0";

    printf("------------------------------------------------------------------------------\n");
    printf("HARSHFS_NODE LISTING:\n  "
           "- CURRENT_NODE: %p\n  "
           "- NAME: %s\n  "
           "- SIZE: %i\n  "
           "- NODE_TYPE: %s\n  "
           "- NEXT_CURRENT_DIR: %p\n  "
           "- PREVIOUS_CURRENT_DIR: %p\n  "
           "- NEXT_DIR: %p\n  "
           "- PREVIOUS_DIR: %p\n  "
           "- DATA: ",
           ((u32*)address),
           image_copy_struct->name,
           image_copy_struct->size,
           ((image_copy_struct->harshfs_node_type == FILE_CODE) ? file : directory),
           image_copy_struct->next_in_current_dir,
           image_copy_struct->previous_in_current_dir,
           image_copy_struct->next_dir,
           image_copy_struct->previous_dir);

    if (image_copy_struct->harshfs_node_type == FILE_CODE) {
        if (address_of_data != NULL) {
            print_data((u8*)address_of_data, (image_copy_struct->size));
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
//    printf("Size: %u = %#4x\nImage Location Start: %p\nFS Address: %#4x\nFinal File Address: %#4x\nNode Size: %#4x\n", overall_fs_size, overall_fs_size, image, filesystem_address, final_file_address, sizeof(harshfs_node));

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

        memcpy(real_node->name, node->name, 256);
        real_node->harshfs_node_type = node->harshfs_node_type;
        real_node->size = node->size;
        real_node->next_in_current_dir = (harshfs_node*)next_in_current_dir;
        real_node->previous_in_current_dir = (harshfs_node*)previous_in_current_dir;
        real_node->next_dir = (harshfs_node*)next_dir;
        real_node->previous_dir = (harshfs_node*)previous_dir;

        if (node->data && node->size) {
            real_node->data = (u8*)final_file_address;
            memcpy((image_copy+(final_file_address - 0xF0000000)), node->data, (node->size+8));
            filesystem_address += node->size+8;
            final_file_address += node->size+8;
        } else {
            real_node->data = NULL;
        }

        memcpy(image, real_node, sizeof(harshfs_node));
        harshfs_node* image_copy_struct = (harshfs_node*) image;
        print_node_binary(image_copy_struct, ((image-image_copy)+0xF0000000), node->data);

        image += sizeof(harshfs_node);

        free(real_node);

        node_address = node_address->next;
    }

//    printf("Size: %u = %#4x\nImage Location Start: %p\nFS Address: %#4x\nFinal File Address: %#4x\nNode Size: %#4x\n", overall_fs_size, overall_fs_size, image, filesystem_address, final_file_address, sizeof(harshfs_node));


    image = image_copy+(final_file_address - 0xF0000000);

//    printf("Place Footer Address: %p\n", image);

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
    printf("\n\n");

    FILE *fptr;
    fptr = fopen("/src/HarshOS/build_os/harshfs_kernel_initial_image.bin","wb");

    if (!fptr) {
        printf("ERROR -- CANNOT WRITE HARSHFS IMAGE!");
        return;
    }



    fwrite(image_copy, 1, overall_fs_size, fptr);

    free(image_copy);


    fclose(fptr);
}

void read_harshfs_image(char* filename) {

    printf("\n\nREADING HARSHFS IMAGE:\n\n");

    FILE *fptr;
    fptr = fopen(filename,"rb");

    if (!fptr) {
        printf("ERROR -- CANNOT READ HARSHFS IMAGE!");
        return;
    }

    fseek(fptr, 0L, SEEK_END);
    u32 sz = ftell(fptr);

    fclose(fptr);

    fptr = NULL;
    fptr = fopen(filename,"rb");

    u8* get_file = (u8*) malloc(sz);
    u8* get_file_copy = get_file;
    memset(get_file, 0, sz);

    fread(get_file, 1, sz, fptr);

    fclose(fptr);

    for (u32 i = 0; i < overall_fs_size; i++) {
        printf("%1x", (*(get_file + i)));
    }
    printf("\n\n");
    for (u32 i = 0; i < overall_fs_size; i++) {
        printf("%c", (*(get_file + i)));
    }
    printf("\n");

    get_file += 4;

    u32 latest_size = 0;
    while ((get_file - get_file_copy) <= (overall_fs_size - 4) && get_header_or_footer(get_file) != FILESYSTEM_FOOTER_MAGIC_NUMBER) {

        if (get_header_or_footer(get_file) == ALL_FILES_HEADER_MAGIC_NUMBER) {
            get_file += latest_size;
//            printf("------------------------------------------------------------------------------\n"
//                           "******************************** DATA_REGION: ********************************\n");
//            printf("header: %4x\nlatest_size: %u\n",get_header_or_footer(get_file),latest_size);
//            print_data(get_file, latest_size);
//            printf("------------------------------------------------------------------------------\n");
//            get_file += (latest_size+8);
        } else {
            harshfs_node* node = (harshfs_node*) get_file;
            latest_size += node->size+8;
            print_node_binary(node, ((get_file - get_file_copy) + 0xF0000000), (u8*)((((u32)node->data) - 0xF0000000)+get_file_copy));
            get_file += sizeof(harshfs_node);
        }
    }

    free(get_file_copy);
}

// Use empty char buffer
u32 get_file_binary(char* filename, u8** buffer) {

    printf("\n\nREADING EXISTING FILE: %s\n\n", filename);

    FILE *fptr;
    fptr = fopen(filename,"rb");

    if (fptr == NULL) return NULL;

    fseek(fptr, 0L, SEEK_END);
    u32 size = ftell(fptr);

    fclose(fptr);

    fptr = NULL;
    fptr = fopen(filename,"rb");

    *buffer = (u8*) malloc(size);
    memset(*buffer, 0, size);

    fread(*buffer, 1, size, fptr);

    fclose(fptr);

    return size;
}

void free_all_memory() {

    harshfs_node_address* node = ROOT_NODE_ADDRESS;

    while (node) {
        harshfs_node_address* next = node->next;
        free(node->node->data);
        free(node->node);
        free(node);
        node = next;
    }

}

int main() {

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

    harshfs_node* inner_directory = add_directory(ROOT_NODE, "dir1", &error_code);
    print_node(node, error_code);

//    harshfs_node* node_copy = node;
    node = add_file(inner_directory, "test_dir1.txt\0", &error_code, create_data((u8*)"Hello World", 11));
    print_node(node, error_code);

    node = add_file(inner_directory, "test_dir1.txt\0", &error_code, NULL);
    print_node(node, error_code);

    node = add_file(inner_directory, "test_dir2.txt\0", &error_code, create_data((u8*)"My name is sriharsha singam", 27));
    print_node(node, error_code);

    u8* buffer = NULL;
    u32 size_of_file = get_file_binary("/src/HarshOS/initrd/hello_world", &buffer);

//    for (u32 i = 0; i < size_of_file; i++) {
//        printf("%1x", (*(buffer+i)));
//    }
//    printf("\n");
    if (size_of_file > 0) {
        node = add_file(inner_directory, "test_binary.bin\0", &error_code, create_data(buffer, size_of_file));
        free(buffer);
        print_node(node, error_code);
    } else {
        printf("------------------------------------------------------------------------------\n");
        printf("ERROR: TEST BINARY FILE NOT FOUND\n");
        printf("------------------------------------------------------------------------------\n");
    }


    filesystem_address += 4;
    overall_fs_size += 4;

    printf("*************FINISHED - Creating Kernel HarshFS Initial Filesystem**************\n");
    printf("|------------------------------------------------------------------------------|\n");

    creating_image();
    read_harshfs_image("/src/HarshOS/build_os/harshfs_kernel_initial_image.bin");

    free_all_memory();

    return 0;
}
#pragma GCC diagnostic pop