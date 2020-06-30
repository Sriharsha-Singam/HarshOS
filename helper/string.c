//
// Created by Sriharsha Singam on 5/24/20.
//

#include "string.h"

#include "../drivers/screen_control.h"
int string_length(char val[]) {
    int counter = 0;
    while (val[counter] != '\0') {
        counter++;
    }
    return counter;
}

void string_append(char val[], char add) {
    int len = string_length(val);
    val[len++] = add;
    val[len] = '\0';
}

void string_concat(char val1[], char val2[]) {
    int length = string_length(val1);
    int i = 0;
    do {
        val1[length+i] = val2[i];
    } while (val2[i++] != '\0');
}

int string_compare_fixed_length(char val1[], char val2[], u32 length) {
    for (u32 i = 0; i < length; i++) {
        if (val1[i] != val2[i]) return 1;
    }
    return 0;
}

int string_compare(char val1[], char val2[]) {
    int i;
    for (i = 0; val1[i] == val2[i]; i++) {
        if (val1[i] == '\0') return 0;
    }
    return val1[i] - val2[i];
}

int reverse_string_at(char string_val[], int from, int to) {

    int i = to - from;
    if (i < 0) return -1;

    for (int j = 0; j < i/2; j++) {
        char c = string_val[j];
        string_val[j] = string_val[i-j-1];
        string_val[i-j-1] = c;
    }
    return 0;
}

void reverse_string(char string_val[]) {
    int i = 0;
    while (string_val[i]) ++i;
    reverse_string_at(string_val, 0, i);
}

void int_to_ascii(int val, char array[]) {
    int i = 0, sign = 0;
    if (val < 0) {
        val = -val;
        sign = 1;
    }

    do {
        array[i++] = (val % 10) + 0x30;
    } while ((val/=10) > 0);

    if (sign) array[i++] = '-';
    array[i] = '\0';

    reverse_string(array);
}

void u8_to_hex_ascii(u8 val, char array[]) {
    int i = 0;

    do {
        if ((val % 16) < 10) {array[i++] = (val % 16) + 0x30;}
        else {array[i++] = ((val % 16) - 10) + 0x41;}
    } while ((val/=16) > 0);

    array[i++] = 'x';
    array[i++] = '0';
    array[i] = '\0';

    reverse_string(array);
}

void u32_to_hex_ascii(u32 val, char array[]) {
    int i = 0;

    do {
        if ((val % 16) < 10) {array[i++] = (val % 16) + 0x30;}
        else {array[i++] = ((val % 16) - 10) + 0x41;}
    } while ((val/=16) > 0);

    array[i++] = 'x';
    array[i++] = '0';
    array[i] = '\0';

    reverse_string(array);
}

u32 hex_ascii_to_u32(char array[]) {

    int i = 2;
    u32 value = 0x0;
    while(array[i] != '\0') {
        value = value << 4;
        if (array[i] < 0x3A) {
            value = value | (array[i] - 0x30);
        } else {
            value = value | (array[i] - 0x37);
        }
        i++;
    }

    return value;
}

int parse_string_spaces(char* values, char value[], u32 index) {

    int string_size = string_length(values);
    char* location_start = values;
    char* location_end = values;

    for (u32 i = 0; i < index; i++) {
        if (string_size < 0) return -1;
        if (i > 0) {
            location_start = ++values;
            string_size--;
        }

        while ((*values != 0x0) && (*values != 0x20) && (*values != 0xA)) {
            location_end = values++;
            string_size--;
            if (string_size < 0) return -1;
        }
    }

    if (location_start >= location_end) return -1;

    u32 size = (u32)(location_end - location_start) + 1;

    u32 counter = 0;
    for (counter = 0;counter < size; counter++) {
        value[counter] = location_start[counter];
    }

    value[++counter] = '\0';

    return 0;
}

u32 number_of_a_chars(char* values, char val) {

    u32 string_numbers = 0;

    while ((*values != 0x0) && (*values != 0xA)) {
        if(*values == val) string_numbers++;
        values++;
    }

    string_numbers++;

    return string_numbers;
}