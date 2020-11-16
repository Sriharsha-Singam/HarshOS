//
// Created by Sriharsha Singam on 6/14/2020.
//

#ifndef HARSHOS_DEFINED_MACROS_H
#define HARSHOS_DEFINED_MACROS_H

#include "../cpu/types.h"

#define ON 1
#define OFF 0

/**
 * Place where all constants are placed
 */
#define KERNEL_SHELL_MODE 0
#define USER_MODE_ENABLED 1

#define DEBUG_MODE ON

#define PAGING_ERROR_NO_PHYSICAL_PAGES 0

/**
 *  Place to specify certain modes to the OS using the above constants.
 */

extern u8 OS_MODE;

void HALT_THE_CPU();
void HALT_THE_CPU_WITH_MESSAGE(char* halt_reason);
void LOG_DEBUG(char* print_text, u32 print_value);
void PANIC(u32 panic_reason);
void PANIC_STRING(char* panic_reason);

#endif //HARSHOS_DEFINED_MACROS_H
