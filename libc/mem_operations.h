//
// Created by Harsh on 5/24/20.
//

#ifndef OS_32_MEM_OPERATIONS_H
#define OS_32_MEM_OPERATIONS_H

#include "../cpu/types.h"

void memory_copy(char* from, char* to, int num_of_bytes);
void memory_set(u8 *dest, u8 val, u32 len);

#endif //OS_32_MEM_OPERATIONS_H
