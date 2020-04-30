#define WHITE_ON_BLACK 0x0F
#define RED_ON_WHITE 0xF4

/* Public kernel API */
void clear_screen();
void kernel_print_string_at(char *message, int col, int row);
void kernel_print_string(char *message);