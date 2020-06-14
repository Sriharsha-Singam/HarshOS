//
// Created by Harsh on 5/24/20.
//

#include "os_shell.h"
#include "../helper/string.h"
#include "./screen_control.h"
#include "./keyboard.h"
#include "../memory/kernel_malloc.h"
#include "../memory/paging.h"

u8 number_of_instructions = 8;

char *kernel_level_instructions[] = {
        "hello\0",
        "shutdown\0",
        "show regs\0",
        "show malloc-addr\0",
        "show value\0",
        "start paging\0",
        "help\0",
        "div-err\0"
};

kernel_level_instructions_function kernel_level_instructions_functions[] = {
        operation_not_found,
        hello_instruction,
        shutdown_instruction,
        show_regs_instruction,
        show_malloc_addr_instruction,
        show_value_instruction,
        start_paging_instruction,
        help_instruction,
        division_error_instruction
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

    if (val == BACKSPACE) {
        if (buffer_length > 0) {
            buffer[--buffer_length] = '\0';
        }
        return;
    } else {
        buffer[buffer_length++] = val;
        buffer[buffer_length] = '\0';
    }


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
    if (string_compare("", buffer)) {
        move_next_line();
        u8 i = 0;
        for (i = 0; i < number_of_instructions; i++) {
            if (!string_compare_fixed_length(kernel_level_instructions[i], buffer, string_length(kernel_level_instructions[i]))) {
                u32 length = string_length(kernel_level_instructions[i]);
                char* args = buffer + length + 1;
                kernel_level_instructions_functions[++i](args);
                end_of_user_input();
                return;
            }
        }
        kernel_level_instructions_functions[0](buffer);
    }
    end_of_user_input();
}

/**
 * This pragma ignore is necessary since every shell function must have
 * char* buffer as a parameter for the function pointer above to work.
 */
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

void show_regs_instruction(char* buffer) {
    u32 eax = 0, ebx = 0, ecx = 0, edx = 0, esp = 0, ebp = 0, esi = 0, edi = 0, cs = 0, ds = 0, es = 0, fs = 0, gs = 0, ss = 0;
    char eaxs[11];
    char ebxs[11];
    char ecxs[11];
    char edxs[11];
    char esps[11];
    char ebps[11];
    char esis[11];
    char edis[11];
    char css[11];
    char dss[11];
    char ess[11];
    char fss[11];
    char gss[11];
    char sss[11];

    asm("movl %%eax, %0" : "=r"(eax) :);
    asm("movl %%ebx, %0" : "=r"(ebx) :);
    asm("movl %%ecx, %0" : "=r"(ecx) :);
    asm("movl %%edx, %0" : "=r"(edx) :);

    asm("movl %%esp, %0" : "=r"(esp) :);
    asm("movl %%ebp, %0" : "=r"(ebp) :);
    asm("movl %%esi, %0" : "=r"(esi) :);
    asm("movl %%edi, %0" : "=r"(edi) :);

    asm("movl %%cs, %0" : "=r"(cs) :);
    asm("movl %%ds, %0" : "=r"(ds) :);
    asm("movl %%es, %0" : "=r"(es) :);
    asm("movl %%fs, %0" : "=r"(fs) :);
    asm("movl %%gs, %0" : "=r"(gs) :);
    asm("movl %%ss, %0" : "=r"(ss) :);

    u32_to_hex_ascii(eax, eaxs);
    u32_to_hex_ascii(ebx, ebxs);
    u32_to_hex_ascii(ecx, ecxs);
    u32_to_hex_ascii(edx, edxs);

    u32_to_hex_ascii(esp, esps);
    u32_to_hex_ascii(ebp, ebps);
    u32_to_hex_ascii(esi, esis);
    u32_to_hex_ascii(edi, edis);

    u32_to_hex_ascii(cs, css);
    u32_to_hex_ascii(ds, dss);
    u32_to_hex_ascii(es, ess);
    u32_to_hex_ascii(fs, fss);
    u32_to_hex_ascii(gs, gss);
    u32_to_hex_ascii(ss, sss);

    kernel_print_string("------------------------------ REGISTERS VALUES ------------------------------\n");
    kernel_print_string("EAX = ");
    kernel_print_string(eaxs);
    kernel_print_string("; EBX = ");
    kernel_print_string(ebxs);
    kernel_print_string("; ECX = ");
    kernel_print_string(ecxs);
    kernel_print_string("; EDX = ");
    kernel_print_string(edxs);
    kernel_print_string("; EIP = ");

    u32 latest_eip = get_latest_eip();
    char eips[11];
    u32_to_hex_ascii(latest_eip, eips);
    kernel_print_string(eips);

    kernel_print_string(";\nESP = ");
    kernel_print_string(esps);
    kernel_print_string("; EBP = ");
    kernel_print_string(ebps);
    kernel_print_string("; ESI = ");
    kernel_print_string(esis);
    kernel_print_string("; EDI = ");
    kernel_print_string(edis);

    kernel_print_string(";\nCS = ");
    kernel_print_string(css);
    kernel_print_string("; DS = ");
    kernel_print_string(dss);
    kernel_print_string("; ES = ");
    kernel_print_string(ess);
    kernel_print_string("; FS = ");
    kernel_print_string(fss);
    kernel_print_string("; GS = ");
    kernel_print_string(gss);
    kernel_print_string("; SS = ");
    kernel_print_string(sss);
}

void show_malloc_addr_instruction(char* buffer) {
    char value[11];
    u32_to_hex_ascii(mallocable_address, value);
    kernel_print_string("The current malloc address is: ");
    kernel_print_string(value);
}

void show_value_instruction(char* buffer) {

    u32* address_to_check = (u32*)hex_ascii_to_u32(buffer);
    u32 address = (u32)address_to_check;

    char value[11];
    u32_to_hex_ascii(address, value);
    char value1[11];
    u32_to_hex_ascii(*address_to_check, value1);

    kernel_print_string("The value at the address ");
    kernel_print_string(value);
    kernel_print_string(" -> ");

    address += 3;
    u32_to_hex_ascii(address, value);

    kernel_print_string(value);
    kernel_print_string(" is : ");
    kernel_print_string(value1);
}

void start_paging_instruction(char* buffer) {
    kernel_print_string("Starting Paging Initialization. Currently creating one page directory meant for the Kernel Operations..........\n");
    //start_paging();
    kernel_print_string("Paging Initialization has completed successfully.");
}

void help_instruction(char* buffer) {
    for (u8 i = 0; i < number_of_instructions; i++) {
        kernel_print_string(" -- ");
        kernel_print_string(kernel_level_instructions[i]);
        if (i != (number_of_instructions - 1)) {
            kernel_print_string("\n");
        }
    }
}

void division_error_instruction(char* buffer) {
    u8 err = 5;
    u8 zero = 0;
    err /= zero;
}

void operation_not_found(char* buffer) {
    kernel_print_string("Kernel Operation ");
    kernel_print_string("\"");
    kernel_print_string(buffer);
    kernel_print_string("\" ");
    kernel_print_string("is Unknown or not Created Yet :)");
}

#pragma GCC diagnostic pop