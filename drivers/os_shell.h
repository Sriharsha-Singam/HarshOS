//
// Created by Harsh on 5/24/20.
//

#ifndef OS_32_OS_SHELL_H
#define OS_32_OS_SHELL_H

#include "../cpu/types.h"

void init_kernel_shell();
void kernel_user_input(u8 val);
void hello_instruction(char* buffer);
void shutdown_instruction(char* buffer);
void show_regs_instruction(char* buffer);
void show_malloc_addr_instruction(char* buffer);
void show_value_instruction(char* buffer);
void start_paging_instruction(char* buffer);
void operation_not_found(char* buffer);
void help_instruction(char* buffer);
void division_error_instruction(char* buffer);

typedef void (*kernel_level_instructions_function)(char* arguments);

#endif //OS_32_OS_SHELL_H
