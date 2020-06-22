#ifndef TYPES_H
#define TYPES_H

typedef unsigned int   u32;
typedef          int   s32;
typedef unsigned short u16;
typedef          short s16;
typedef unsigned char  u8;
typedef          char  s8;
typedef unsigned int  physical_addr;

#define NULL 0

#define low_16(address) (u16)((address) & 0xFFFF)
#define high_16(address) (u16)(((address) >> 16) & 0xFFFF)

#define low_8(address) (u8)((address) & 0xFF)
#define high_8(address) (u8)(((address) >> 8) & 0xFF)

#define bit0_8(address) (u8))((address) & 0b00000001)
#define bit1_8(address) (u8)(((address) & 0b00000010) >> 1)
#define bit2_8(address) (u8)(((address) & 0b00000100) >> 2)
#define bit3_8(address) (u8)(((address) & 0b00001000) >> 3)
#define bit4_8(address) (u8)(((address) & 0b00010000) >> 4)
#define bit5_8(address) (u8)(((address) & 0b00100000) >> 5)
#define bit6_8(address) (u8)(((address) & 0b01000000) >> 6)
#define bit7_8(address) (u8)(((address) & 0b10000000) >> 7)

#endif