//
// Created by Sriharsha Singam on 5/24/20.
//

#ifndef HARSHOS_OS_SHELL_H
#define HARSHOS_OS_SHELL_H

#include "../cpu/types.h"

void init_kernel_shell();
void end_of_user_input();

void* clear_current_caused_kernel_interrupt_pointer();
//void (*end_of_user_input_pointer) ();

//void *end_of_user_input_pointer;
//void (*end_of_user_input_pointer) ();
//end_of_user_input_pointer = &end_of_user_input;

void kernel_user_input(u8 val);

void hello_instruction(char* buffer);
void shutdown_instruction(char* buffer);
void show_regs_instruction(char* buffer);
void show_malloc_addr_instruction(char* buffer);
void show_value_instruction(char* buffer);
void set_value_instruction(char* buffer);
void start_paging_instruction(char* buffer);
void operation_not_found(char* buffer);
void help_instruction(char* buffer);
void division_error_instruction(char* buffer);
void show_page_directory(char* buffer);
void user_mode_on(char* buffer);
void user_mode_off(char* buffer);
void parse_string_instruction(char* buffer);
void heap_entry_instruction(char* buffer);
void heap_length_instruction(char* buffer);
void insert_heap_entry_instruction(char* buffer);
void free_heap_entry_instruction(char* buffer);
void print_heap_entries_instruction(char* buffer);
void print_hex_test_entries(void* address, u8 next_line);
void test_heap_instruction(char* buffer);
void wait_ms_instruction(char* buffer);

typedef void (*kernel_level_instructions_function)(char* arguments);

#endif //HARSHOS_OS_SHELL_H
