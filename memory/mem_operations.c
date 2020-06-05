//
// Created by Harsh on 5/24/20.
//

#include "mem_operations.h"

void memory_copy(char* from, char* to, int num_of_bytes) {
    for (int i = 0; i < num_of_bytes; i++) {
        *( to + i ) = *( from + i );
    }
}

void memory_set(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

