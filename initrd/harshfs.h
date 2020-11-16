//
// Created by Sriharsha Singam on 6/30/2020.
//

#ifndef HARSHOS_HARSHFS_H
#define HARSHOS_HARSHFS_H

#include "../cpu/types.h"

extern u32 filesystem_address;

#define FILE_CODE 0x0
#define DIRECTORY 0X1
#define NOT_USED 0x2

#define FILE_NAME_EXISTS 0x1
#define FILE_PARENT_DIR_NOT_FOUND 0x2
#define UNKNOWN_FILE_TYPE 0x3
#define DATA_INPUT_IS_INVALID 0x4

extern char* errors[];

#define ALL_FILES_HEADER_MAGIC_NUMBER 0x8D32CA15
#define ALL_FILES_FOOTER_MAGIC_NUMBER 0xD918CA12

#define FILESYSTEM_HEADER_MAGIC_NUMBER 0xC9B3FCCB
#define FILESYSTEM_FOOTER_MAGIC_NUMBER 0x5D20603B

#define KERNEL_VIRTUAL_HARSHFS_ADDRESS 0xF0000000
#define KERNEL_PAGE_HARSHFS_NUMBER (KERNEL_VIRTUAL_HARSHFS_ADDRESS >> 22)

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
    u8* data;
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

#ifndef CREATE_KERNEL_INITRD
    void add_file(harshfs_node* parent_directory, char name[], u8* error_code, void* data);
    void add_directory(harshfs_node* parent_directory, char name[], u8* error_code);
    u32 get_header_or_footer(u8* data);
    int check_validity(u8* data);
    u32 get_data_size(u8* data);
    harshfs_node* read_harshfs_kernel_image();
    void load_harshfs_kernel_image();
#endif


#endif //HARSHOS_HARSHFS_H
