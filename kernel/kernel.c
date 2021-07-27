#include "../drivers/screen_control.h"
#include "../cpu/interrupt_handler.h"
#include "../drivers/os_shell.h"
#include "../memory/paging.h"
#include "../helper/string.h"
#include "defined_macros.h"

//extern u32 start;
extern u32 code;

void kernel_main() {

//    HALT_THE_CPU();

    setup_interrupts();

//    HALT_THE_CPU();

    clear_screen();

//    HALT_THE_CPU();

    LOG_DEBUG("Currently Loading The Kernel at address: ", (u32)&code);

    start_paging();

    kernel_print_string("\n\nkernel shell intialization\n");
    init_kernel_shell();

//    LOG_DEBUG("\n\nKernel Main Has Ended. Waiting on Interrupts\n", NULL);
}