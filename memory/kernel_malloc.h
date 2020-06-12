//
// Created by INT_ACC on 6/6/2020.
//

#ifndef HARSHOS_KERNEL_MALLOC_H
#define HARSHOS_KERNEL_MALLOC_H

#include "../cpu/types.h"

extern u32 mallocable_address;
u32 kernel_malloc(u32 mem_size);
u32 kernel_malloc_page_aligned(u32 mem_size);

#endif //HARSHOS_KERNEL_MALLOC_H
