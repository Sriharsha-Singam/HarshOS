#include "timer.h"
#include "../helper/string.h"
#include "../drivers/port_access.h"
#include "../drivers/screen_control.h"
#include "interrupt_handler.h"

u32 tick = 0;

u32 number_of_timer_cycles = 0;

static void timer_callback(interrupt_inputs_t interruptInputs) {

    if (interruptInputs.interrupt_no != IRQ0) return;

    tick++;
    number_of_timer_cycles++;

//    char tick_ascii[256];
//    int_to_ascii(tick, tick_ascii);
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

void wait_ms(u32 number_of_milli_seconds) {
    number_of_timer_cycles = 0;
    u32 number_of_nano_seconds = number_of_milli_seconds*1000000;

    while (number_of_timer_cycles <= number_of_nano_seconds) {}

    return;
}
