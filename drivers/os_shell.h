//
// Created by Harsh on 5/24/20.
//

#ifndef OS_32_OS_SHELL_H
#define OS_32_OS_SHELL_H

#include "../cpu/types.h"

char buffer[256];
u16 buffer_length;
void init_kernel_shell();
void kernel_user_input(u8 val);

#endif //OS_32_OS_SHELL_H
