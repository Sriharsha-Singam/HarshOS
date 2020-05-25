//#include "../drivers/cursor.h"
//#include "./util.h"
#include "../drivers/screen_control.h"
#include "../cpu/interrupt_handler.h"
//#include "../cpu/idt.h"
#include "../drivers/keyboard.h"
#include "../cpu/timer.h"
void print_loader() {
    int i = 0;
    while(i < 25) {
        for (int j = 0; j < 10000000; j++) {}
        kernel_print_string(".");
        i++;
    }
    kernel_print_string("\n");
}
void main() {
    interrupt_service_request_install();

    clear_screen();
//    kernel_print_string("Currently Loaded The Start of Kernel Successfully.\n");
//    kernel_print_string("Loaded Separate I/O Ports");
//    print_loader();
    __asm__ __volatile__("sti");
    init_timer(50);
//    asm volatile("sti");
    init_keyboard();



    /* Test the interrupts */
//    int x = 16/0;
//    __asm__ __volatile__("int $2");
//    __asm__ __volatile__("int $3");


//    kernel_print_string("\nLoaded Cursor Controls");
//    print_loader();
//    kernel_print_string("\nLoaded VGA Screen Driver");
//    print_loader();
//    kernel_print_string("\n");


//    char var[255];
//    int_to_ascii(-1231, var);
//    kernel_print_string(var);
//    kernel_print_string("Currently Loaded The Start of Kernel Successfully.\n");
//    kernel_print_string("Loaded Separate I/O Ports");
//    kernel_print_string("\nLoaded Cursor Controls");
//    kernel_print_string("\nLoaded VGA Screen Driver");
//    kernel_print_string("\n");
//    kernel_print_string("Currently Loaded The Start of Kernel Successfully.\n");
//    kernel_print_string("Loaded Separate I/O Ports");
//    kernel_print_string("\nLoaded Cursor Controls");
//    kernel_print_string("\nLoaded VGA Screen Driver");
//    kernel_print_string("\n");
//    kernel_print_string("567Currently Loaded The Start of Kernel Successfully.\n");
//    kernel_print_string("567Loaded Separate I/O Ports");
//    kernel_print_string("\n567Loaded Cursor Controls");
//    kernel_print_string("\n567Loaded VGA Screen Driver");
//    kernel_print_string("\n");
}
