//
// Created by Sriharsha Singam on 6/30/2020.
//

#include "harshfs.h"
#include "../memory/mem_operations.h"
#include "../memory/kernel_heap.h"
#include "../helper/string.h"

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
