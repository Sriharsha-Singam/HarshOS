//
// Created by Sriharsha Singam on 6/30/2020.
//

#include "harshfs.h"
#include "../memory/mem_operations.h"
#include "../memory/kernel_heap.h"
#include "../helper/string.h"

harshfs_node* ROOT_NODE = NULL;

harshfs_node* insert_harshfs_node(harshfs_node* parent_directory, u8 file_type, char name[], u8* error_code) {

    if (parent_directory == NULL) {
        if (ROOT_NODE == NULL) {
            ROOT_NODE = kernel_heap_calloc(sizeof(harshfs_node), 0, NULL);
            ROOT_NODE->harshfs_node_type = DIRECTORY;
            for (u32 i = 0; i < string_length(name); i++) {
                if (i == 256) break;
                ROOT_NODE->name[i] = name[i];
            }
        } else {
            return NULL;
        }
    }

    if (file_type == DIRECTORY) {
        harshfs_node* new_directory = kernel_heap_calloc(sizeof(harshfs_node), 0, NULL);
        if (parent_directory->next_dir == NULL) {
            parent_directory->next_dir = new_directory;
            ((harshfs_directory_data*)(new_directory->data))->FIRST_NODE = NULL;
            ((harshfs_directory_data*)(new_directory->data))->LAST_NODE = NULL;
        } else {
            harshfs_node* directory = parent_directory->next_dir;
            while (directory) {
                if (!string_compare(directory->name, name)) {
                    *error_code = FILE_NAME_EXISTS;
                    kernel_heap_free(new_directory);
                    return NULL;
                }
                directory = directory->next_in_current_dir;
            }

        }
    }

    harshfs_node* new_node = kernel_heap_calloc(sizeof(harshfs_node), 0, NULL);
    harshfs_node* parent_copy = parent_directory;

    while(parent_copy->next_in_current_dir) {
        parent_copy = parent_copy->next_in_current_dir;
    }
    parent_copy->next_in_current_dir = new_node;
    new_node->previous_in_current_dir = parent_copy;
    new_node->next_in_current_dir = NULL;
    new_node->

}

void remove_harshfs_node(harshfs_node* remove) {

}

void add_file(harshfs_node* parent_directory, char name[]) {
    insert_harshfs_node(parent_directory, FILE, name);
}

void add_directory(harshfs_node* parent_directory, char name[]) {
    insert_harshfs_node(parent_directory, DIRECTORY, name);
}