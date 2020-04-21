#include "port_access.h"

unsigned char port_byte_read(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

unsigned short port_word_read(unsigned short port) {
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_write(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data) , "d" (port));
}

void port_word_write(unsigned short port, unsigned short data) {
    __asm__("out %%ax, %%dx" : : "a" (data) , "d" (port));
}