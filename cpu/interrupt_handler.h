//
// Created by Sriharsha Singam on 5/17/20.
//

#ifndef HARSHOS_INTERRUPT_HANDLER_H
#define HARSHOS_INTERRUPT_HANDLER_H

#include "types.h"
#include "../kernel/defined_macros.h"

/* ISRs reserved for CPU exceptions */
extern void interrupt_service_request0();
extern void interrupt_service_request1();
extern void interrupt_service_request2();
extern void interrupt_service_request3();
extern void interrupt_service_request4();
extern void interrupt_service_request5();
extern void interrupt_service_request6();
extern void interrupt_service_request7();
extern void interrupt_service_request8();
extern void interrupt_service_request9();
extern void interrupt_service_request10();
extern void interrupt_service_request11();
extern void interrupt_service_request12();
extern void interrupt_service_request13();
extern void interrupt_service_request14();
extern void interrupt_service_request15();
extern void interrupt_service_request16();
extern void interrupt_service_request17();
extern void interrupt_service_request18();
extern void interrupt_service_request19();
extern void interrupt_service_request20();
extern void interrupt_service_request21();
extern void interrupt_service_request22();
extern void interrupt_service_request23();
extern void interrupt_service_request24();
extern void interrupt_service_request25();
extern void interrupt_service_request26();
extern void interrupt_service_request27();
extern void interrupt_service_request28();
extern void interrupt_service_request29();
extern void interrupt_service_request30();
extern void interrupt_service_request31();

extern void interrupt_request_pic0();
extern void interrupt_request_pic1();
extern void interrupt_request_pic2();
extern void interrupt_request_pic3();
extern void interrupt_request_pic4();
extern void interrupt_request_pic5();
extern void interrupt_request_pic6();
extern void interrupt_request_pic7();
extern void interrupt_request_pic8();
extern void interrupt_request_pic9();
extern void interrupt_request_pic10();
extern void interrupt_request_pic11();
extern void interrupt_request_pic12();
extern void interrupt_request_pic13();
extern void interrupt_request_pic14();
extern void interrupt_request_pic15();

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

//extern char* exception_messages[32];

/* Struct which aggregates many registers */
typedef struct {
    u32 ds; /* Data segment selector */
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
    u32 interrupt_no, error_code; /* Interrupt number and error code (if applicable) */
    u32 eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} interrupt_inputs_t;

typedef void (*interrupt_handler)(interrupt_inputs_t);

void setup_interrupts();

void interrupt_service_request_handler(interrupt_inputs_t interruptInputs);
void interrupt_request_pic_handler(interrupt_inputs_t interruptInputs);

void set_interrupt_handler(u8 interrupt_number, interrupt_handler handler);

#endif //HARSHOS_INTERRUPT_HANDLER_H
