/* Screen i/o port registers */
#define REG_VGA_ACTION 0x3D4
#define REG_VGA_DATA 0x3D5
#define VIDEO_ADDRESS 0xc00b8000

/* Max rows and cols */
#define MAX_ROWS 25
#define MAX_COLS 80

int get_cursor_location_offset();
void set_cursor_location_offset(int offset);

int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
char* get_vga_address(int col, int row);