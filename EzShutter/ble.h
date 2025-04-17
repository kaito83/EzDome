#ifndef BLE_H
#define BLE_H

#include <Arduino.h>;

namespace ble {
  void init();
  void available();
  void subscribe();
  void tx(String cmd);
  String rx();
  bool rx_upd();
  void poll();
}

#endif
