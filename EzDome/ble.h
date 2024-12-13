#ifndef ble_h
#define ble_h

void ble_init();
void ble_tx(char cmd_type, String cmd_val);
String ble_rx();
void ble_central_connected();
#endif