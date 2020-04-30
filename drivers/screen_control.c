#include "cursor.h"
#include "../kernel/util.h"
#include "screen_control.h"

int print_single_char(char c, int col, int row, char attr);

void clear_screen() {
    int overall_screen_size = MAX_ROWS * MAX_COLS;
    char *vga = VIDEO_ADDRESS;

    int i = 0;
    for (i = 0; i < overall_screen_size; i++) {
        *(vga + i*2) = ' ';
        *(vga + (i*2 + 1)) = WHITE_ON_BLACK;
    }

    set_cursor_location_offset(get_offset(0, 0));
}

void kernel_print_string_at(char* message, int col, int row) {

    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    } else {
        offset = get_cursor_location_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i = 0;
    while (message[i] != 0) {
        offset = print_single_char(message[i++], col, row, WHITE_ON_BLACK);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

}

int print_single_char(char c, int col, int row, char attr) {
    unsigned char *vga = (unsigned char*) VIDEO_ADDRESS;
    int current_offset;

    if (col >= MAX_COLS || row >= MAX_ROWS) {
        vga[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vga[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    } else if (col >= 0 && row >= 0) {
        current_offset = get_offset(col, row);
    } else {
        current_offset = get_cursor_location_offset();
    }

    if (c == '\n') {
        row = get_offset_row(current_offset);
        current_offset = get_offset(0, (row+1));
    } else {
        vga[current_offset++] = c;
        vga[current_offset++] = attr;
    }

    if (current_offset >= (MAX_COLS*MAX_ROWS*2)) {
        for (int i = 1; i < MAX_ROWS; i++){
            memory_copy(get_vga_address(0, i), get_vga_address(0, i-1), MAX_COLS * 2);
        }
        char *last_line = get_vga_address(0, MAX_ROWS-1);
        for (int i = 0; i < MAX_COLS * 2; i++) last_line[i] = 0;

        current_offset -= 2 * MAX_COLS;
    }

    set_cursor_location_offset(current_offset); //setting new offset
    return current_offset;
}



void kernel_print_string(char *message) {
    kernel_print_string_at(message, -1, -1);
    return;
}