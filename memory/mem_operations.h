//
// Created by Sriharsha Singam on 5/24/20.
//

#ifndef HARSHOS_MEM_OPERATIONS_H
#define HARSHOS_MEM_OPERATIONS_H

#include "../cpu/types.h"

void memory_copy(char* from, char* to, int num_of_bytes);
void memory_set(u32 *dest, u8 val, u32 len);

#endif //HARSHOS_MEM_OPERATIONS_H
