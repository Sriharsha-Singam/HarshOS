//
// Created by Harsh on 5/24/20.
//

#include "string.h"

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