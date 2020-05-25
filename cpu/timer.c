#include "timer.h"
#include "../kernel/util.h"
#include "../drivers/port_access.h"
#include "../drivers/screen_control.h"
#include "interrupt_handler.h"

u32 tick = 0;

static void timer_callback(interrupt_inputs_t regs) {
    tick++;
//    kernel_print_string("Tick: ");
    
    char tick_ascii[256];
    int_to_ascii(tick, tick_ascii);
//    kernel_print_string(tick_ascii);
//    kernel_print_string("\n");
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    set_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */

    port_byte_write(0x43, 0x36); /* Command port */
    port_byte_write(0x40, low);
    port_byte_write(0x40, high);
}
