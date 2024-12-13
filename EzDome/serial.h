#ifndef serial_h
#define serial_h
void init_serial();
int serial_available();
String serial_readStringUntil();
void serial_out(char cmd_type, String cmd_val);
void ser_popbuf();
#endif