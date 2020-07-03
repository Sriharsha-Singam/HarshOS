//
// Created by Sriharsha Singam on 6/30/2020.
//

#ifndef HARSHOS_HARSHFS_H
#define HARSHOS_HARSHFS_H

#include "../cpu/types.h"

#define FILE 0x0
#define DIRECTORY 0X1

#define FILE_NAME_EXISTS 0x1

//typedef struct _harshfs_node_basic harshfs_node_basic;
//
//struct _harshfs_node_basic {
//    char name[256];               // File Name
//    u8 harshfs_node_type;         // The type -- FILE or DIRECTORY
//    u32 size;                     // Size of the current directory/size
//    harshfs_node* next_in_current_dir;      // Next file/dir in the Current Directory
//    harshfs_node* previous_in_current_dir;  // Previous file/dir in the Current Directory
//};



typedef struct _harshfs_node harshfs_node;

// TODO: Make LINKED LIST implementation FROM HEAP more usable in future case to have simple LINKED LIST functions accessible
// TODO: Need to add ACCESS CONTROL flags
// TODO: Somehow account for DATA in the case of a File
struct _harshfs_node {
    char name[256];               // File Name
    u8 harshfs_node_type;         // The type -- FILE or DIRECTORY
    u32 size;                     // Size of the current directory/size
    harshfs_node* next_in_current_dir;      // Next file/dir in the Current Directory
    harshfs_node* previous_in_current_dir;  // Previous file/dir in the Current Directory
    harshfs_node* next_dir;       // Next Directory inside the Current Directory
    harshfs_node* previous_dir;   // Previous Directory inside the Current Directory
    void* data;
};

typedef struct _harshfs_directory_data harshfs_directory_data;

// TODO: Make LINKED LIST implementation FROM HEAP more usable in future case to have simple LINKED LIST functions accessible
// TODO: Need to add ACCESS CONTROL flags
// TODO: Somehow account for DATA in the case of a File
struct _harshfs_directory_data {
    harshfs_node* FIRST_NODE;
    harshfs_node* LAST_NODE;
};

//typedef struct _harshfs_node_directory harshfs_node_directory;
//
//struct _harshfs_node_directory {
//    struct harshfs_node_basic basic;
//    harshfs_node* next_dir;       // Next Directory inside the Current Directory
//    harshfs_node* previous_dir;   // Previous Directory inside the Current Directory
//};
//
//typedef struct _harshfs_node_file harshfs_node_file;
//
//struct _harshfs_node_file {
//    struct harshfs_node_basic basic;
//    void* data;
//};


#endif //HARSHOS_HARSHFS_H
