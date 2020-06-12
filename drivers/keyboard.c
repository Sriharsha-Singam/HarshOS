#include "port_access.h"
#include "../cpu/interrupt_handler.h"
#include "screen_control.h"
#include "../helper/string.h"
#include "keyboard.h"
#include "./os_shell.h"

void print_letter(u8 scancode);

u8 shift_down = 0;
u8 ctrl_down = 0;
u8 alt_down = 0;

const char shift_up_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
                          '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
                          'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
                          'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
                          'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '};

const char shift_down_ascii[] = { '?', '?', '!', '@', '#', '$', '%', '^',
                          '&', '*', '(', ')', '_', '+', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
                          'U', 'I', 'O', 'P', '{', '}', '?', '?', 'A', 'S', 'D', 'F', 'G',
                          'H', 'J', 'K', 'L', ':', '\"', '~', '?', '|', 'Z', 'X', 'C', 'V',
                          'B', 'N', 'M', '<', '>', '?', '?', '?', '?', ' '};

static void keyboard_callback(interrupt_inputs_t input) {
    /* The PIC leaves us the scancode in port 0x60 */

    if (input.interrupt_no != IRQ1) return;
    u8 scancode1 = port_byte_read(0x60);
    print_letter(scancode1);
}

void init_keyboard() {
    set_interrupt_handler(IRQ1, keyboard_callback);
}

//void shell_print(u8 scancode) {
//    u8 letter = keyboard_scancode_handler(scancode);
//    if (letter!=0xA) if ((letter >=0 && letter < 32) || letter >= 0x7F) return;
//
//    if (letter == 0xA)
//        kernel_user_input()
//}

void print_letter(u8 scancode) {
    u8 letter = keyboard_scancode_handler(scancode);
    if (letter!=0xA && letter!=0x8) if ((letter < 32) || letter >= 0x7F) return;

//    char text[2];
//    text[0] = letter;
//    text[1] = '\0';

//    kernel_print_string(text);
    kernel_user_input(letter);
}
u8 keyboard_scancode_handler(u8 scancode) {

    switch(scancode) {
        case 0x1C:
            return 0xA; // Next Line
        case 0x2A:
        case 0x36:
            shift_down = 1;
            return 0xF;
        case 0xAA:
        case 0xB6:
            shift_down = 0;
            return 0xE;
        case 0x1D:
            ctrl_down = 1;
            return 0xFF;
        case 0x9D:
            ctrl_down = 0;
            return 0xFF;
        case 0x38:
            alt_down = 1;
            return 0xFF;
        case 0xB8:
            alt_down = 0;
            return 0xFF;
        case 0x1:
            return 0x1B; // Escape
        case 0xE:
            backspace();
            return 0x8; // Backspace
        case 0xF:
            tab();
            return 0x9; // Tab
        case 0x4B:
            move_cursor_left();
            return 0xFF;
        case 0x4D:
            move_cursor_right();
            return 0xFF;
        case 0x48:
            move_cursor_up();
            return 0xFF;
        case 0x50:
            move_cursor_down();
            return 0xFF;
    }

    if (scancode <= (sizeof(shift_down_ascii) / sizeof(shift_down_ascii[0]))) {
        if (shift_down) return (u8)shift_down_ascii[scancode];
        else return (u8)shift_up_ascii[scancode];
    }

    return 0xFF;
}
