#ifndef ble_h
#define ble_h

void ble_init();
void ble_available();
void ble_subscribe();
void ble_tx(String cmd);
String ble_rx();
bool ble_rx_upd();
void ble_poll();

#endif