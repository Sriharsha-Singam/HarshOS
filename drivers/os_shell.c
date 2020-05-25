//
// Created by Harsh on 5/24/20.
//

#include "os_shell.h"
#include "../libc/string.h"
#include "./screen_control.h"

char buffer[256];
u16 buffer_length;
void next_kernel_shell_print();
void end_of_user_input();
void user_shell_enter();
void hello_instruction();
void operation_not_found();

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
    char hello[6] = "hello\0";
    if (!string_compare(hello, buffer)) hello_instruction();
    else operation_not_found();
    end_of_user_input();
}

void hello_instruction() {
    char hello[29] = "------ Hello World :) ------\0";
    kernel_print_string(hello);
}

void operation_not_found() {
    kernel_print_string("Kernel Operation Not Created Yet :)");
}