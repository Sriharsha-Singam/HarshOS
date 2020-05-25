//#include "../drivers/cursor.h"
//#include "./util.h"
#include "../drivers/screen_control.h"
#include "../cpu/interrupt_handler.h"
//#include "../cpu/idt.h"
#include "../drivers/keyboard.h"
#include "../cpu/timer.h"
#include "../drivers/os_shell.h"

void main() {
    setup_interrupts();
    clear_screen();
    kernel_print_string("Currently Loading The Kernel.");
    kernel_old_print_loader();
    kernel_print_string("\n\nkernel shell intialization\n");
    init_kernel_shell();
}