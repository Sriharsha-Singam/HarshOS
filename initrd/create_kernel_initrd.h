//
// Created by INT_ACC on 7/5/2020.
//

#ifndef HARSHOS_CREATE_KERNEL_INITRD_H
#define HARSHOS_CREATE_KERNEL_INITRD_H

#define CREATE_KERNEL_INITRD 0x1

#include "harshfs.h"

typedef struct _harshfs_node_address harshfs_node_address;

struct _harshfs_node_address {
    harshfs_node* node;
    u32 address;
    harshfs_node_address* next;
};

#endif //HARSHOS_CREATE_KERNEL_INITRD_H
