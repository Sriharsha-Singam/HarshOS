//
// Created by Harsh on 5/24/20.
//

#include "os_shell.h"
#include "../helper/string.h"
#include "./screen_control.h"

u8 number_of_instructions = 2;

char *kernel_level_instructions[] = {
        "hello",

        "shutdown"
};

kernel_level_instructions_function kernel_level_instructions_functions[] = {
        operation_not_found,
        hello_instruction,
        shutdown_instruction
};

char buffer[256];
u16 buffer_length;
void next_kernel_shell_print();
void end_of_user_input();
void user_shell_enter();

void init_kernel_shell() {
    buffer[0] = '\0';
    buffer_length = 0;
    next_kernel_shell_print();
}

void next_kernel_shell_print() {
    for (int i = 0; i < 256; i++) buffer[i] = 0;
    buffer_length = 0;
    kernel_print_string("sriharsha_os:shell kernel# ");
}

void kernel_user_input(u8 val) {
    if (val == 0xA) {
        user_shell_enter();
        return;
    }
    buffer[buffer_length++] = val;
    buffer[buffer_length] = '\0';

    char text[2];
    text[0] = val;
    text[1] = '\0';
    kernel_print_string(text);
}

void end_of_user_input() {
    move_next_line();
    next_kernel_shell_print();
}
void user_shell_enter() {
    move_next_line();
    u8 i = 0;
    for (i = 0; i < number_of_instructions; i++) {
        if (!string_compare(kernel_level_instructions[i], buffer)) {
            kernel_level_instructions_functions[++i](buffer);
            end_of_user_input();
            return;
        }
    }
    kernel_level_instructions_functions[0](buffer);
    end_of_user_input();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/** These are the available kernel instructions implementation
 *  TODO: These can be used in the kernel state or with kernel privileges
 *  TODO: Add more necessary kernel instructions
 */

void hello_instruction(char* buffer) {
    char hello[29] = "------ Hello World :) ------\0";
    kernel_print_string(hello);
}

void shutdown_instruction(char* buffer) {
    kernel_print_string("Shutting Down the CPU. Good Bye :)");
    asm volatile("hlt");
}

void operation_not_found(char* buffer) {
    kernel_print_string("Kernel Operation Not Created Yet :)");
}

#pragma GCC diagnostic pop