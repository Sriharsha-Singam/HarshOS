//
// Created by Harsh on 5/17/20.
//

#include "interrupt_handler.h"
#include "idt.h"
#include "../helper/string.h"
#include "../drivers/port_access.h"
#include "../drivers/screen_control.h"
#include "timer.h"
#include "../drivers/keyboard.h"

#define PIC_INIT 0x11

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC1_IRQ_OFFSET 0x20  // 0x20 = 32 which is because the Master PIC refers to Interrupt Numbers 32-39
#define PIC2_IRQ_OFFSET 0x28 // 0x28 = 40 which is because the Slave PIC refers to Interrupt Numbers 40-45

#define SLAVE_MASTER_IRQ_INPUT 0x4 // Tell Master PIC that the Slave PIC's input into the Master slave is located at
                                   // the IRQ2 of the Master PIC which is (0000 0100) -- including IRQ0

#define SLAVE_CASCADE_IDENTITY 0x2 // ???

#define PIC_EOI 0x20 // Must send EOI to Master and/or Slave PIC when interrupt handler completes


interrupt_handler interrupt_handlers[256];


/* Can't do this with a loop because we need the address
 * of the function names */
void interrupt_service_request_install() {
    set_idt_interrupt_entry(0, (u32)interrupt_service_request0);
    set_idt_interrupt_entry(1, (u32)interrupt_service_request1);
    set_idt_interrupt_entry(2, (u32)interrupt_service_request2);
    set_idt_interrupt_entry(3, (u32)interrupt_service_request3);
    set_idt_interrupt_entry(4, (u32)interrupt_service_request4);
    set_idt_interrupt_entry(5, (u32)interrupt_service_request5);
    set_idt_interrupt_entry(6, (u32)interrupt_service_request6);
    set_idt_interrupt_entry(7, (u32)interrupt_service_request7);
    set_idt_interrupt_entry(8, (u32)interrupt_service_request8);
    set_idt_interrupt_entry(9, (u32)interrupt_service_request9);
    set_idt_interrupt_entry(10, (u32)interrupt_service_request10);
    set_idt_interrupt_entry(11, (u32)interrupt_service_request11);
    set_idt_interrupt_entry(12, (u32)interrupt_service_request12);
    set_idt_interrupt_entry(13, (u32)interrupt_service_request13);
    set_idt_interrupt_entry(14, (u32)interrupt_service_request14);
    set_idt_interrupt_entry(15, (u32)interrupt_service_request15);
    set_idt_interrupt_entry(16, (u32)interrupt_service_request16);
    set_idt_interrupt_entry(17, (u32)interrupt_service_request17);
    set_idt_interrupt_entry(18, (u32)interrupt_service_request18);
    set_idt_interrupt_entry(19, (u32)interrupt_service_request19);
    set_idt_interrupt_entry(20, (u32)interrupt_service_request20);
    set_idt_interrupt_entry(21, (u32)interrupt_service_request21);
    set_idt_interrupt_entry(22, (u32)interrupt_service_request22);
    set_idt_interrupt_entry(23, (u32)interrupt_service_request23);
    set_idt_interrupt_entry(24, (u32)interrupt_service_request24);
    set_idt_interrupt_entry(25, (u32)interrupt_service_request25);
    set_idt_interrupt_entry(26, (u32)interrupt_service_request26);
    set_idt_interrupt_entry(27, (u32)interrupt_service_request27);
    set_idt_interrupt_entry(28, (u32)interrupt_service_request28);
    set_idt_interrupt_entry(29, (u32)interrupt_service_request29);
    set_idt_interrupt_entry(30, (u32)interrupt_service_request30);
    set_idt_interrupt_entry(31, (u32)interrupt_service_request31);

//    u8 original_pic1_data = port_byte_read(PIC1_DATA);
//    u8 original_pic2_data = port_byte_read(PIC2_DATA);

    port_byte_write(PIC1_COMMAND, PIC_INIT);
    port_byte_write(PIC2_COMMAND, PIC_INIT);

    port_byte_write(PIC1_DATA, PIC1_IRQ_OFFSET);
    port_byte_write(PIC2_DATA, PIC2_IRQ_OFFSET);

    port_byte_write(PIC1_DATA, SLAVE_MASTER_IRQ_INPUT);
    port_byte_write(PIC2_DATA, SLAVE_CASCADE_IDENTITY);

    port_byte_write(PIC1_DATA, 0x1);
    port_byte_write(PIC2_DATA, 0x1);

    port_byte_write(PIC1_DATA, 0x0);
    port_byte_write(PIC2_DATA, 0x0);

    // PIC Master
    set_idt_interrupt_entry(32, (u32)interrupt_request_pic0);
    set_idt_interrupt_entry(33, (u32)interrupt_request_pic1);
    set_idt_interrupt_entry(34, (u32)interrupt_request_pic2);
    set_idt_interrupt_entry(35, (u32)interrupt_request_pic3);
    set_idt_interrupt_entry(36, (u32)interrupt_request_pic4);
    set_idt_interrupt_entry(37, (u32)interrupt_request_pic5);
    set_idt_interrupt_entry(38, (u32)interrupt_request_pic6);
    set_idt_interrupt_entry(39, (u32)interrupt_request_pic7);

    // PIC Slave
    set_idt_interrupt_entry(40, (u32)interrupt_request_pic8);
    set_idt_interrupt_entry(41, (u32)interrupt_request_pic9);
    set_idt_interrupt_entry(42, (u32)interrupt_request_pic10);
    set_idt_interrupt_entry(43, (u32)interrupt_request_pic11);
    set_idt_interrupt_entry(44, (u32)interrupt_request_pic12);
    set_idt_interrupt_entry(45, (u32)interrupt_request_pic13);
    set_idt_interrupt_entry(46, (u32)interrupt_request_pic14);
    set_idt_interrupt_entry(47, (u32)interrupt_request_pic15);

    load_idt_register(); // Load with ASM
}

void interrupt_request_pic_install() {
    /* Enable interruptions */
    asm volatile("sti");
    /* IRQ0: timer */
    init_timer(50);
    /* IRQ1: keyboard */
    init_keyboard();
}

void setup_interrupts() {
    interrupt_service_request_install();
    interrupt_request_pic_install();
}

char *exception_messages[] = {
        "Division By Zero",
        "Debug",
        "Non Maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",

        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bad TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",

        "Coprocessor Fault",
        "Alignment Check",
        "Machine Check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",

        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

void interrupt_service_request_handler(interrupt_inputs_t interruptInputs) {
    kernel_print_string("Interrupt Has Been Recieved: ");
    char interrupt_number_string[3];
    int_to_ascii(interruptInputs.interrupt_no, interrupt_number_string);
    kernel_print_string(interrupt_number_string);
    kernel_print_string("\n");
    kernel_print_string(exception_messages[interruptInputs.interrupt_no]);
    kernel_print_string("\n\n");
}

void set_interrupt_handler(u8 interrupt_number, interrupt_handler handler) {
    interrupt_handlers[interrupt_number] = handler;
}

void interrupt_request_pic_handler(interrupt_inputs_t interruptInputs) {

    /**
     *  PIC EOI must ALWAYS be sent to the PIC Master
     *  PIC EOI only needs to be sent to the PIC Slave when the interrupt comes from  the slave
     */

    if (interruptInputs.interrupt_no <= 47) {
        interrupt_handler handler = interrupt_handlers[interruptInputs.interrupt_no];
        handler(interruptInputs);
    }

    if (interruptInputs.interrupt_no >= 40) port_byte_write(PIC2_COMMAND, PIC_EOI);
    port_byte_write(PIC1_COMMAND, PIC_EOI);

}