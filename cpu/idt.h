#ifndef IDT_H
#define IDT_H

#include "types.h"

/** This is the byte offset of the index in the GDT. 0x8 is referring to the first
 *  entry in the GDT which is the overall Code Segment for NOW --
 *
 *  TODO: make a kernel and user-space differentiation in code/data segments.
 */
#define KERNEL_SEGMENT_SELECTORS 0X8

typedef struct {
    u16 low_offset; /** Lower 16 bits of handler function address */
    u16 kernel_segment_selector; /** KERNEL SEGMENT SELECTOR FOR THE KERNEL*/
    u8 always_zero; /** This must always be zero */
    /** First byte
     * Bit 7: is Interrupt Present
     * Bits 6-5: Privilege Level (0=highest-privilege & 3=lowest-privilege)
     * Bit 4: Set to 0 for interrupt gates
     * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    u8 flags;
    u16 high_offset; /** Higher 16 bits of handler function address */
} __attribute__((packed)) idt_entry_t ;

/**
 * This contains the structure/register for the IDT pointer
 * location. This pointer will be used for the
 * ASM INSTRUCTION: "lidt"
 *
 * Refer: https://www.inf.ed.ac.uk/teaching/courses/os/slides/09-memory18.pdf
 *     |-> Page 5
 */
typedef struct {
    u16 limit; /** Number of bytes in the IDT data structure */
    u32 base; /** This is the base address of the IDT data structure */
} __attribute__((packed)) idt_register_t;

#define NUMBER_OF_IDT_INTERRUPTS 256 // There can be 256 interrupts to the kernel

/**
 *  Functions to SET IDT entries and set the IDT register value
 */
void set_idt_interrupt_entry(int n, u32 handler); //Set IDT Interrupt Entry
void load_idt_register(); // Load IDT register

#endif
