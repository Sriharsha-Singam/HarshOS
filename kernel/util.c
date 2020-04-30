#include "util.h"

void memory_copy(char* from, char* to, int num_of_bytes) {
    for (int i = 0; i < num_of_bytes; i++) {
        *( to + i ) = *( from + i );
    }
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