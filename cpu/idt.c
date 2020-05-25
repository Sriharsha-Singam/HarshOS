#include "idt.h"
#include "../kernel/util.h"

void set_idt_interrupt_entry(int n, u32 handler) {
    /**
     * Set the Address for the IDT Interrupt
     * Handler
     */
    idt[n].high_offset = high_16(handler);
    idt[n].low_offset = low_16(handler);

    idt[n].kernel_segment_selector = KERNEL_SEGMENT_SELECTORS;
    idt[n].always_zero = 0x0;

    /** First byte
    * Bit 7: is Interrupt Present
    * Bits 6-5: Privilege Level (0=highest-privilege & 3=lowest-privilege)
    * Bit 4: Set to 0 for interrupt gates
    * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    idt[n].flags = 0b10001110;
}

void load_idt_register() {

    /** This is setting the base of the IDT Entries */
    idt_reg.base = (u32) &idt;

    /** This is calculating the size necessary for all the possible Interrupt Entries */
    idt_reg.limit = NUMBER_OF_IDT_INTERRUPTS * sizeof(idt_entry_t) - 1;


    /**
     *  Loading the IDT Register address into the LIDTL x86
     *  https://c9x.me/x86/html/file_module_x86_id_156.html
     */
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_reg));
}