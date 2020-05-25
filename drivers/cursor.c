#include "port_access.h"
#include "cursor.h"

int get_cursor_location_offset() {

    // READING VGA I/O PORTS: http://www.osdever.net/FreeVGA/vga/crtcreg.htm

    // Reading Cursor Location High Register
    port_byte_write(REG_VGA_ACTION, 0xE);
    unsigned char upper_vga_cursor_location = port_byte_read(REG_VGA_DATA);

    // Reading Cursor Location Low Register
    port_byte_write(REG_VGA_ACTION, 0xF);
    unsigned char lower_vga_cursor_location = port_byte_read(REG_VGA_DATA);

    int vga_cursor_location = 0x0;
    vga_cursor_location += upper_vga_cursor_location;
    vga_cursor_location = vga_cursor_location << 8;
    vga_cursor_location += lower_vga_cursor_location;

    return (vga_cursor_location * 2); // Each vga cell consists of 2 registers.
}

void set_cursor_location_offset(int offset) {

    offset /= 2;

    // Writing New Cursor Location Low Register
    port_byte_write(REG_VGA_ACTION, 0xF);
    port_byte_write(REG_VGA_DATA, (unsigned char)(offset & 0xff));

    // Writing New Cursor Location High Register
    port_byte_write(REG_VGA_ACTION, 0xE);
    port_byte_write(REG_VGA_DATA, (unsigned char)(offset >> 8));
}

int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}
int get_offset_row(int offset) {
    return (offset / (2 * MAX_COLS));
}
int get_offset_col(int offset) {
    return ((offset - (get_offset_row(offset)*2*MAX_COLS))/2);
}

char* get_vga_address(int col, int row) {
    return ((char *)(get_offset(col, row) + VIDEO_ADDRESS));
}