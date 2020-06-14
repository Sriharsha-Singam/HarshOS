//
// Created by Harsh on 5/19/20.
//

#ifndef OS_32_KEYBOARD_H
#define OS_32_KEYBOARD_H

void init_keyboard();
u8 keyboard_scancode_handler(u8 scancode);
u32 get_latest_eip();

#define BACKSPACE 0x8

#endif //OS_32_KEYBOARD_H
