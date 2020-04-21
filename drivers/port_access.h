// Read data from seperate (not memory-mapped) I/O port -- Read DATA Register
unsigned char port_byte_read(unsigned short port);
unsigned short port_word_read(unsigned short port);

// Write data to seperate (not memory-mapped) I/O port -- DATA Regsiter
void port_byte_write(unsigned short port, unsigned char data);
void port_word_write(unsigned short port, unsigned short data);