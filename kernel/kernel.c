//#include "../drivers/cursor.h"
//#include "./util.h"
//#include "../cpu/idt.h"
#include "../drivers/screen_control.h"
#include "../cpu/interrupt_handler.h"
//#include "../drivers/keyboard.h"
//#include "../cpu/timer.h"
#include "../drivers/os_shell.h"
#include "../memory/paging.h"
#include "../helper/string.h"

//extern u32 start;
extern u32 code;

void main() {
    //start_paging();
    setup_interrupts();

    clear_screen();
//    start_paging();
    kernel_print_string("Currently Loading The Kernel at address: ");

    char value[11];
    u32_to_hex_ascii((u32)&code, value);
    kernel_print_string(value);

    //kernel_old_print_loader();
    kernel_print_string("\n\nkernel shell intialization\n");
    init_kernel_shell();
}