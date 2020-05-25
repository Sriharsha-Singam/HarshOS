#include "port_access.h"
#include "../cpu/interrupt_handler.h"
#include "screen_control.h"
#include "../kernel/util.h"
#include "keyboard.h"

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

//const char keys

static void keyboard_callback(interrupt_inputs_t input) {
    /* The PIC leaves us the scancode in port 0x60 */

    if (input.interrupt_no != IRQ1) return;
    u8 scancode1 = port_byte_read(0x60);
    print_letter(scancode1);
//    char *sc_ascii1 = "";
//
//    u8_to_hex_ascii(scancode1, sc_ascii1);

//    kernel_print_string(sc_ascii1);

//    char *next = "\n\0";
//    u8 scancode2 = port_byte_read(0x61);
//    u8 scancode3 = port_byte_read(0x62);
//    u8 scancode4 = port_byte_read(0x63);
//
//    char *sc_ascii2 = "";
//    char *sc_ascii3 = "";
//    char *sc_ascii4 = "";
//
//    int_to_ascii(scancode2, sc_ascii2);
//    int_to_ascii(scancode3, sc_ascii3);
//    int_to_ascii(scancode4, sc_ascii4);
//
//    kernel_print_string(sc_ascii1);

//    char *test = "\n";
//    sc_ascii1 = "\n\0";
//    kernel_print_string(next);
//    kernel_print_string("\n");
//    kernel_print_string(sc_ascii2);
//    kernel_print_string("\n");
//    kernel_print_string(sc_ascii3);
//    kernel_print_string("\n");
//    kernel_print_string(sc_ascii4);
//    kernel_print_string("\n");
//    kernel_print_string("Keyboard scancode: ");

//    kernel_print_string(", ");
//    print_letter(scancode1);
//    kernel_print_string();
}

void init_keyboard() {
    set_interrupt_handler(IRQ1, keyboard_callback);
}

void print_letter(u8 scancode) {
    u8 letter = keyboard_scancode_handler(scancode);
    if (letter!=0xA) if ((letter >=0 && letter < 32) || letter >= 0x7F) return;

    char text[2];
//    u8_to_hex_ascii(scancode, text);
//    kernel_print_string(text);

//    u8_to_hex_ascii(letter, text);
    text[0] = letter;
    text[1] = '\0';
    kernel_print_string(text);
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
    }

    if (scancode <= (sizeof(shift_down_ascii) / sizeof(shift_down_ascii[0]))) {
        if (shift_down) return (u8)shift_down_ascii[scancode];
        else return (u8)shift_up_ascii[scancode];
    }

    return 0xFF;
}
//void print_letter(u8 scancode) {
//    switch (scancode) {
//        case 0x0:
//            kernel_print_string("ERROR");
//            break;
//        case 0x1:
//            kernel_print_string("ESC");
//            break;
//        case 0x2:
//            kernel_print_string("1");
//            break;
//        case 0x3:
//            kernel_print_string("2");
//            break;
//        case 0x4:
//            kernel_print_string("3");
//            break;
//        case 0x5:
//            kernel_print_string("4");
//            break;
//        case 0x6:
//            kernel_print_string("5");
//            break;
//        case 0x7:
//            kernel_print_string("6");
//            break;
//        case 0x8:
//            kernel_print_string("7");
//            break;
//        case 0x9:
//            kernel_print_string("8");
//            break;
//        case 0x0A:
//            kernel_print_string("9");
//            break;
//        case 0x0B:
//            kernel_print_string("0");
//            break;
//        case 0x0C:
//            kernel_print_string("-");
//            break;
//        case 0x0D:
//            kernel_print_string("+");
//            break;
//        case 0x0E:
////            kernel_print_string("Backspace");
//            backspace();
//            break;
//        case 0x0F:
//            kernel_print_string("    ");
//            break;
//        case 0x10:
//            kernel_print_string("Q");
//            break;
//        case 0x11:
//            kernel_print_string("W");
//            break;
//        case 0x12:
//            kernel_print_string("E");
//            break;
//        case 0x13:
//            kernel_print_string("R");
//            break;
//        case 0x14:
//            kernel_print_string("T");
//            break;
//        case 0x15:
//            kernel_print_string("Y");
//            break;
//        case 0x16:
//            kernel_print_string("U");
//            break;
//        case 0x17:
//            kernel_print_string("I");
//            break;
//        case 0x18:
//            kernel_print_string("O");
//            break;
//        case 0x19:
//            kernel_print_string("P");
//            break;
//        case 0x1A:
//            kernel_print_string("[");
//            break;
//        case 0x1B:
//            kernel_print_string("]");
//            break;
//        case 0x1C:
//            kernel_print_string("\n");
//            break;
//        case 0x1D:
//            kernel_print_string("LCtrl");
//            break;
//        case 0x1E:
//            kernel_print_string("A");
//            break;
//        case 0x1F:
//            kernel_print_string("S");
//            break;
//        case 0x20:
//            kernel_print_string("D");
//            break;
//        case 0x21:
//            kernel_print_string("F");
//            break;
//        case 0x22:
//            kernel_print_string("G");
//            break;
//        case 0x23:
//            kernel_print_string("H");
//            break;
//        case 0x24:
//            kernel_print_string("J");
//            break;
//        case 0x25:
//            kernel_print_string("K");
//            break;
//        case 0x26:
//            kernel_print_string("L");
//            break;
//        case 0x27:
//            kernel_print_string(";");
//            break;
//        case 0x28:
//            kernel_print_string("'");
//            break;
//        case 0x29:
//            kernel_print_string("`");
//            break;
//        case 0x2A:
//            shift = 1;
////            kernel_print_string("LShift");
//            break;
//        case 0x2B:
//            kernel_print_string("\\");
//            break;
//        case 0x2C:
//            kernel_print_string("Z");
//            break;
//        case 0x2D:
//            kernel_print_string("X");
//            break;
//        case 0x2E:
//            kernel_print_string("C");
//            break;
//        case 0x2F:
//            kernel_print_string("V");
//            break;
//        case 0x30:
//            kernel_print_string("B");
//            break;
//        case 0x31:
//            kernel_print_string("N");
//            break;
//        case 0x32:
//            kernel_print_string("M");
//            break;
//        case 0x33:
//            kernel_print_string(",");
//            break;
//        case 0x34:
//            kernel_print_string(".");
//            break;
//        case 0x35:
//            if (shift == 1) {
//                kernel_print_string("?");
//                shift = 0;
//                break;
//            }
//            kernel_print_string("/");
//            break;
//        case 0x36:
//            shift = 1;
////            kernel_print_string("Rshift");
//            break;
//        case 0x37:
//            kernel_print_string("Keypad *");
//            break;
//        case 0x38:
//            kernel_print_string("LAlt");
//            break;
//        case 0x39:
//            kernel_print_string(" ");
//            break;
//        default:
//            /* 'keuyp' event corresponds to the 'keydown' + 0x80
//             * it may still be a scancode we haven't implemented yet, or
//             * maybe a control/escape sequence */
////            if (scancode <= 0x7f) {
////                kernel_print_string("Unknown key down");
////            } else if (scancode <= 0x39 + 0x80) {
////                kernel_print_string("key up ");
////                print_letter(scancode - 0x80);
////            } else kernel_print_string("Unknown key up");
//            break;
//    }
//}
