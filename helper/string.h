//
// Created by Sriharsha Singam on 5/24/20.
//

#ifndef HARSHOS_STRING_H
#define HARSHOS_STRING_H

#include "../cpu/types.h"

int string_length(char val[]);
void string_append(char val[], char add);
void string_concat(char val1[], char val2[]);
int string_compare(char val1[], char val2[]);
int string_compare_fixed_length(char val1[], char val2[], u32 length);
void reverse_string(char string_val[]);
void int_to_ascii(int val, char array[]);
void u8_to_hex_ascii(u8 val, char array[]);
int reverse_string_at(char string_val[], int from, int to);
void u32_to_hex_ascii(u32 val, char array[]);
u32 hex_ascii_to_u32(char array[]);
int parse_string_spaces(char* values, char dvalue[], u32 index);
u32 number_of_a_chars(char* values, char val);

#endif //HARSHOS_STRING_H
