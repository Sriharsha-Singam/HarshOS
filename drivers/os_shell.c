//
// Created by Sriharsha Singam on 5/24/20.
//

#include "os_shell.h"
#include "../cpu/timer.h"
#include "../helper/string.h"
#include "./screen_control.h"
#include "./keyboard.h"
#include "../memory/kernel_malloc.h"
#include "../memory/paging.h"
#include "../kernel/defined_macros.h"
#include "../memory/mem_operations.h"
#include "../memory/kernel_heap.h"
#include "../initrd/harshfs.h"

u8 number_of_instructions = 21;

//void (*end_of_user_input_pointer)() = end_of_user_input;

char *kernel_level_instructions[] = {
        "help\0",
        "hello\0",
        "shutdown\0",
        "show regs\0",
        "show malloc-addr\0",
        "show value\0",
        "set value\0",
        "start paging\0",
        "div-err\0",
        "show page-dir\0",
        "user-mode on\0",
        "user-mode off\0",
        "parse string\0",
        "heap entry\0",
        "heap length\0",
        "i\0",
        "f\0",
        "print heap\0",
        "test heap\0",
        "wait_ms\0",
        "harshfs\0"
};

//"free heap entry\0",
//"insert heap entry\0",

kernel_level_instructions_function kernel_level_instructions_functions[] = {
        operation_not_found,
        help_instruction,
        hello_instruction,
        shutdown_instruction,
        show_regs_instruction,
        show_malloc_addr_instruction,
        show_value_instruction,
        set_value_instruction,
        start_paging_instruction,
        division_error_instruction,
        show_page_directory,
        user_mode_on,
        user_mode_off,
        parse_string_instruction,
        heap_entry_instruction,
        heap_length_instruction,
        insert_heap_entry_instruction,
        free_heap_entry_instruction,
        print_heap_entries_instruction,
        test_heap_instruction,
        wait_ms_instruction,
        harshfs_instruction
};

char buffer[256];

u32 index_for_instruction_history_buffer = 0;
u32 current_index_for_instruction_history_buffer = 0;
u32 max_instruction_history = 10;
char *instruction_history_buffer[10];

u16 buffer_length;
void next_kernel_shell_print();
void end_of_user_input();
void user_shell_enter();

void init_kernel_shell() {

//    end_of_user_input_pointer = end_of_user_input;

    buffer[0] = '\0';
    buffer_length = 0;
    next_kernel_shell_print();
}

void next_kernel_shell_print() {
    for (int i = 0; i < 256; i++) buffer[i] = 0;
    buffer_length = 0;
    kernel_print_string("sriharsha_os:shell kernel# ");
}

void user_shell_up_or_down(u8 up_or_down) {

    shell_erase_line();

    if (up_or_down == UP && index_for_instruction_history_buffer >= current_index_for_instruction_history_buffer) {
        if (current_index_for_instruction_history_buffer != 0) current_index_for_instruction_history_buffer--;
        char* instruction_show = instruction_history_buffer[current_index_for_instruction_history_buffer];
        u32 length = string_length(instruction_show);
        memory_copy(instruction_show, &buffer[0], length+1);
        buffer_length = length;
        kernel_print_string(buffer);
    } else if (up_or_down == DOWN && index_for_instruction_history_buffer >= current_index_for_instruction_history_buffer+1 && current_index_for_instruction_history_buffer <= max_instruction_history-1) {
        if (index_for_instruction_history_buffer != current_index_for_instruction_history_buffer) {
            current_index_for_instruction_history_buffer++;
        }
        if (index_for_instruction_history_buffer == current_index_for_instruction_history_buffer) {
            buffer[0] = '\0';
            buffer_length = 0;
            kernel_print_string(buffer);
        } else {
            char* instruction_show = instruction_history_buffer[current_index_for_instruction_history_buffer];
            u32 length = string_length(instruction_show);
            memory_copy(instruction_show, &buffer[0], length+1);
            buffer_length = length;
            kernel_print_string(buffer);
        }
    }

}

void kernel_user_input(u8 val) {
    if (val == 0xA) {
        user_shell_enter();
        return;
    }

    if (val == UP || val == DOWN) {
        user_shell_up_or_down(val);
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

/**
 * THIS IS BAD CODE. THIS CODE WILL LET NESTED_EXCEPTIONS STAY ACTIVE IN THE STACK. BY USING THIS FUNCTION
 * THE STACK WILL SLOWLY GET EATEN UP.
 *
 * TODO: Change Method of handling Interrupts
 * TODO:   -- Kernel Interrupts should crash the entire system after saving state for error reporting
 * TODO:   -- User Mode Interrupts should kill process and state why process was killed.
 */
void clear_current_caused_kernel_interrupt() {
    kernel_print_string("DANGEROUS: Causing Software Errors/Interrupts can cause a bad OS Start.\nThis will cause Stack Instability! Beware.");
    end_of_user_input();
    asm volatile("pop %eax");
    asm volatile("popa");
    asm volatile("add $8, %esp");
    asm volatile("sti");
    for(;;) {}
}

/**
 * THIS IS BAD CODE. THIS CODE WILL LET NESTED_EXCEPTIONS STAY ACTIVE IN THE STACK. BY USING THIS FUNCTION
 * THE STACK WILL SLOWLY GET EATEN UP.
 *
 * TODO: Change Method of handling Interrupts
 * TODO:   -- Kernel Interrupts should crash the entire system after saving state for error reporting
 * TODO:   -- User Mode Interrupts should kill process and state why process was killed.
 */
void* clear_current_caused_kernel_interrupt_pointer() {
    return clear_current_caused_kernel_interrupt;
}

void end_of_user_input() {
    move_next_line();
    next_kernel_shell_print();
}

void pop_instruction_history() {
    instruction_history_buffer[0] = NULL;
    for (u32 i = 1; i < max_instruction_history; i++) {
        instruction_history_buffer[i-1] = instruction_history_buffer[i];
        instruction_history_buffer[i] = NULL;
        instruction_history_buffer[i] = instruction_history_buffer[i+1];
    }
}

void push_instruction_history() {
    u32 length = string_length(buffer);
    length++;
    char* instruction_pointer = (char*)kernel_heap_calloc(length, 0, NULL);
    for (u32 i = 0; i < length; i++) {
        *(instruction_pointer + i) = *(buffer + i);
    }

    if (index_for_instruction_history_buffer >= max_instruction_history) {
        pop_instruction_history();
        instruction_history_buffer[max_instruction_history-1] = instruction_pointer;
        current_index_for_instruction_history_buffer = 10;
    } else {
        instruction_history_buffer[index_for_instruction_history_buffer] = instruction_pointer;
        index_for_instruction_history_buffer++;
        current_index_for_instruction_history_buffer = index_for_instruction_history_buffer;
    }
}

void user_shell_enter() {
    if (string_compare("", buffer)) {
        push_instruction_history();
        move_next_line();
        for (u32 i = 0; i < number_of_instructions; i++) {
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
#pragma GCC diagnostic ignored "-Wunused-variable"

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

    kernel_print_string(";\nEIP = ");

    u32 latest_eip = get_latest_eip();
    char eips[11];
    u32_to_hex_ascii(latest_eip, eips);
    kernel_print_string(eips);
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

    kernel_print_string("The value at the address ");
    kernel_print_hex_value((u32)address_to_check);
    kernel_print_string(" -> ");

    address += 3;

    kernel_print_hex_value(address);
    kernel_print_string(" is : ");
    kernel_print_hex_value(*address_to_check);
}

void set_value_instruction(char* buffer) {

    char args[11];
    memory_set((u32*)args, 0, 11);
    parse_string_spaces(buffer, args, 1);

    u32* address = (u32*)hex_ascii_to_u32(args);

    memory_set((u32*)args, 0, 11);
    parse_string_spaces(buffer, args, 2);
    u32 value_to_set = hex_ascii_to_u32(args);

    *address = value_to_set;

    if (*address == value_to_set) kernel_print_string("The new value has successfully been set");
    else kernel_print_string("ERROR: New value was not set properly");
}

void start_paging_instruction(char* buffer) {
    kernel_print_string("Paging Initialization has already been completed successfully.");
}

void help_instruction(char* buffer) {
    for (u8 i = 0; i < number_of_instructions; i++) {
//        kernel_print_string(" -- ");
        kernel_print_string(kernel_level_instructions[i]);

        if (i != (number_of_instructions - 1)) {
            kernel_print_string(", ");
//            kernel_print_string("\n");
        }
    }
}

void division_error_instruction(char* buffer) {
    u8 err = 5;
    u8 zero = 0;
    err /= zero;
}

void show_page_directory(char* buffer) {
    u32 current_directory = 0;
    asm volatile("mov %%cr3, %0" : "=r"(current_directory));
    kernel_print_string("Current Page Directory Address Stored in CR3: ");
    kernel_print_hex_value(current_directory);
}

void user_mode_on(char* buffer) {
    kernel_print_string("WARNING: This is turning on User-Mode features during Kernel-Mode. This may have unintended consequences.\n");
    OS_MODE = USER_MODE_ENABLED;
}

void user_mode_off(char* buffer) {
    kernel_print_string("WARNING: Turning Off User-Mode. Good Choice Lol.");
    OS_MODE = KERNEL_SHELL_MODE;
}

void parse_string_instruction(char* buffer) {
    char arg[256];

    u32 number_of_spaces = number_of_a_chars(buffer, 0x20);

    for (u32 i = 0; i <= number_of_spaces; i++) {
        memory_set((u32*)arg, 0, 256);
        if (parse_string_spaces(buffer, arg, i) == 0) {
            kernel_print_string(arg);
            kernel_print_string("\n");
        }
    }
}

void heap_entry_instruction(char* buffer) {

    u32 index = hex_ascii_to_u32(buffer);

    if (index < length_of_heap_entries_list()) {

        heap_entry_linked_list* heap_entry = get_heap_entry(index);
//        kernel_print_hex_value((u32)heap_entry);
        u32 magic = heap_entry->magic_number;
        u8 is_used = heap_entry->is_used;
        void* heap_location = heap_entry->heap_entry_location;
        u32 size = heap_entry->heap_entry_size;

        kernel_print_string("Heap Entry Linked List Address: ");
        kernel_print_hex_value((u32)heap_entry);
        kernel_print_string("\nMagic Number: ");
        kernel_print_hex_value(magic);
        kernel_print_string("\nHeap Entry Location: ");
        kernel_print_hex_value((u32)heap_location);
        kernel_print_string("\nSize: ");
        kernel_print_hex_value(size);
        kernel_print_string(" ; ");
        if (is_used == USED) kernel_print_string("USED\n");
        else kernel_print_string("FREE\n");

        heap_entry_footer_t* footer = (heap_entry_footer_t*) ((u32)heap_entry - 8);
        kernel_print_hex_value((u32)footer);
        kernel_print_string(" ; ");
        kernel_print_hex_value((u32)footer->magic_number);
        kernel_print_string(" ; ");
        kernel_print_hex_value((u32)footer->header_location);
        kernel_print_string(" ; ");


    } else {
        kernel_print_string("Heap Entry Index Does Not Exist");
    }
}

void heap_length_instruction(char* buffer) {
    kernel_print_string("Length Of Heap Entry Linked List: ");
    kernel_print_hex_value(length_of_heap_entries_list());
}

void insert_heap_entry_instruction(char* buffer) {

    if (buffer[0] == 'p') {
        char* val = buffer + 2;
        u32 size = hex_ascii_to_u32(val);
        void* address = kernel_heap_malloc_page_aligned(size, NULL);
        print_hex_test_entries(address, 0);
        return;
    }

    u32 size = hex_ascii_to_u32(buffer);
    void* address = kernel_heap_malloc(size, NULL);

    print_hex_test_entries(address, 0);
}

void free_heap_entry_instruction(char* buffer) {

    void* address = (void*) hex_ascii_to_u32(buffer);
    kernel_print_hex_value((u32)address);
    kernel_heap_free(address);

    if (does_heap_entry_exist(address) == FREE) {
        kernel_print_string("Heap Entry Has Been FREED Successfully. The Address is: ");
        kernel_print_hex_value((u32)address);
    }
    else kernel_print_string("ERROR: Heap Entry Was NOT Successfully FREED.");

}

void print_hex_test_entries(void* address, u8 next_line) {
    if (does_heap_entry_exist(address)) {
        heap_entry_linked_list* heap_entry = get_heap_entry_address(address);
        kernel_print_string("Heap Entry |Size(");
        kernel_print_hex_value(heap_entry->heap_entry_size);
        kernel_print_string(")| Has Successfully Been Created. Address: ");
        kernel_print_hex_value((u32)address);
    }
    else kernel_print_string("ERROR: Heap Entry Was NOT Successfully Been Created.");

    if (next_line) kernel_print_string("\n");
}

void test_heap_instruction(char* buffer) {

    if (!string_compare(buffer, "test1\0")) {
        print_hex_test_entries(kernel_heap_malloc(0x1, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x2, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x3, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x5, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x100, NULL), 1);

        print_heap_entries_instruction("\0");
        return;
    }

    if (!string_compare(buffer, "test2\0")) {
        print_hex_test_entries(kernel_heap_malloc(0x110, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x34, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x1140, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x5, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc(0x100, NULL), 1);

        print_heap_entries_instruction("\0");

        kernel_print_string("free 0xC0017018\n");
        free_heap_entry_instruction("0xC0017018\0");
        kernel_print_string("free 0xC0017148\n");
        free_heap_entry_instruction("0xC0017148\0");

        kernel_print_string("test heap merge\n");
        test_heap_instruction("merge\0");
        kernel_print_string("test heap sort\n");
        test_heap_instruction("sort\0");
        return;
    }

    if (!string_compare(buffer, "page1\0")) {
        print_hex_test_entries(kernel_heap_malloc(0x1, NULL), 1);
        print_hex_test_entries(kernel_heap_malloc_page_aligned(0x1000, NULL), 1);
        print_heap_entries_instruction("\0");
        print_hex_test_entries(kernel_heap_malloc_page_aligned(0x132, NULL), 1);
        print_heap_entries_instruction("\0");
        print_hex_test_entries(kernel_heap_malloc(0x194, NULL), 1);
        print_heap_entries_instruction("\0");
        print_hex_test_entries(kernel_heap_malloc_page_aligned(0x1, NULL), 1);
        print_heap_entries_instruction("\0");
        return;
    }

    if (!string_compare(buffer, "sort\0")) {
        sort_heap_entry_linked_list();
        print_all_heap_entries();
        return;
    }

    if (!string_compare(buffer, "merge\0")) {
        merge_heap_entries_in_entire_linked_list();
        print_all_heap_entries();
        return;
    }

    // PARSE STRINGS FOR REST OF TEST OPTIONS
//
//    char arg_buffer[256];
//
//    u32 number_of_spaces = number_of_a_chars(buffer, 0x20);
//
//    memory_set((u32*)arg_buffer, 0, 256);
//    if (parse_string_spaces(buffer, arg_buffer, 0) == 0) {
//
//        /**
//         * SWAP ENTRIES CAN ONLY BE DONE FOR 2 SIDE-BY-SIDE ENTRIES
//         */
//        if (!string_compare(arg_buffer, "swap\0")) {
//            char arg1[11], arg2[11];
//
//            parse_string_spaces(buffer, arg1, 1);
//            parse_string_spaces(buffer, arg2, 2)
//
//            swap_heap_entry_linked_list(get_heap_entry_address((void*) hex_ascii_to_u32(arg1)), get_heap_entry_address((void*) hex_ascii_to_u32(arg2)));
//            print_all_heap_entries();
//            return;
//        }
//
//    }

    kernel_print_string("No Available Test Option has been entered. Options Include:\n - test1\n - test2\n - sort\n - merge\n - swap");
}

void print_heap_entries_instruction(char* buffer) {

    if (!string_compare(buffer, "full\0")) {
        print_all_full_heap_entries();
        return;
    }

    print_all_heap_entries();
}

void wait_ms_instruction(char* buffer) {
    u32 ms = hex_ascii_to_u32(buffer);
    wait_ms(ms);
}

void harshfs_instruction(char* buffer) {

    if (!string_compare(buffer, "init\0")) {
        load_harshfs_kernel_image();
        harshfs_node* root_node =  read_harshfs_kernel_image();
//        kernel_print_string("Root Node: ");
//        kernel_print_string(root_node->name);
//        kernel_print_string("\n");
        return;
    }

}

void operation_not_found(char* buffer) {
    kernel_print_string("Kernel Operation ");
    kernel_print_string("\"");
    kernel_print_string(buffer);
    kernel_print_string("\" ");
    kernel_print_string("is Unknown or not Created Yet :)");
}

#pragma GCC diagnostic pop