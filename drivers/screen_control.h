#define WHITE_ON_BLACK 0x0F
#define RED_ON_WHITE 0xF4

#include "../cpu/types.h"

/* Public kernel API */
void clear_screen();
void kernel_print_string_at(char *message, int col, int row);
void kernel_print_string(char *message);
void backspace();
void tab();
void move_cursor_left();
void move_cursor_right();
void move_cursor_up();
void move_cursor_down();
void kernel_old_print_loader();
void move_next_line();
void kernel_print_hex_value(u32 hex_value);
void shell_erase_line();