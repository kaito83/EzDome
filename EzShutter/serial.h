#ifndef serial_h
#define serial_h
void init_serial();
int serial_available();
void serial_out(char cmd_type, String cmd_val, bool ble);
String serial_readStringUntil();
#endif