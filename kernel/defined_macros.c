//
// Created by INT_ACC on 6/15/2020.
//

#include "defined_macros.h"
#include "../drivers/screen_control.h"

u8 OS_MODE = KERNEL_SHELL_MODE;

void HALT_THE_CPU() {
    for(;;) {
        asm volatile("cli");
        asm volatile("hlt");
    }
}

void HALT_THE_CPU_WITH_MESSAGE(char* halt_reason) {
    kernel_print_string(halt_reason);
    kernel_print_string("\n");
    HALT_THE_CPU();
}

void LOG_DEBUG(char* print_text, u32 print_value) {
    if (DEBUG_MODE == OFF) return;
    kernel_print_string("DEBUG: ");
    kernel_print_string(print_text);
    if (print_value) kernel_print_hex_value((u32)print_value);
    kernel_print_string("\n");
}

void PANIC(u32 panic_reason) {
    if (panic_reason == PAGING_ERROR_NO_PHYSICAL_PAGES) {
        HALT_THE_CPU_WITH_MESSAGE("PAGING ERROR: No Physical Pages Available To Allocate.");
    }
}

void PANIC_STRING(char* panic_reason) {
    HALT_THE_CPU_WITH_MESSAGE(panic_reason);
}