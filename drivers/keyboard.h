//
// Created by Sriharsha Singam on 5/19/20.
//

#ifndef HARSHOS_KEYBOARD_H
#define HARSHOS_KEYBOARD_H

void init_keyboard();
u8 keyboard_scancode_handler(u8 scancode);
u32 get_latest_eip();

#define BACKSPACE 0x8
#define UP 0x48
#define DOWN 0x50

#endif //HARSHOS_KEYBOARD_H
