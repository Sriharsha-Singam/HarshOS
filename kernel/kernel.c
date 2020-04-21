//#include "../drivers/cursor.h"
#include "../drivers/screen_control.h"

void main() {
    clear_screen();
    kernel_print_string("Currently Loaded The Start of Kernel Successfully.\n");
    kernel_print_string("Loaded Separate I/O Ports\n");
    kernel_print_string("Loaded Cursor Controls\n");
    kernel_print_string("Loaded VGA Screen Driver\n");
}

//    clear_screen();
//    char *message = "Hello";
//    kernel_print_string("Hello");
//    char *vga = VIDEO_ADDRESS;
//    *vga = 'X';
//    *(vga+2) = 'Y';
//    clear_screen();

//    set_cursor_location_offset((get_cursor_location_offset()+18));
//    set_cursor_location_offset(get_offset(0,0));

//      char* message = "ABCDEFGHI";
//      kernel_print_string_at(message, 0, 0);
//    int offset_of_cursor_from_vga = get_cursor_location_offset();
//    vga[offset_of_cursor_from_vga] = 'A';
//    vga[offset_of_cursor_from_vga+2] = 'B';
//    vga[offset_of_cursor_from_vga+4] = 'C';
//    vga[offset_of_cursor_from_vga+6] = 'D';
//    vga[offset_of_cursor_from_vga+8] = 'E';
//    vga[offset_of_cursor_from_vga+10] = 'F';
//    vga[offset_of_cursor_from_vga+12] = 'G';
//    vga[offset_of_cursor_from_vga+14] = 'H';
//    vga[offset_of_cursor_from_vga+16] = 'I';