//
// Created by Harsh on 5/24/20.
//

#ifndef OS_32_OS_SHELL_H
#define OS_32_OS_SHELL_H

#include "../cpu/types.h"

void init_kernel_shell();
void kernel_user_input(u8 val);
void hello_instruction();
void shutdown_instruction();
void operation_not_found();

typedef void (*kernel_level_instructions_function)(char* arguments);

#endif //OS_32_OS_SHELL_H
