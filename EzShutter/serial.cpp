#include "serial.h";
#include <Arduino.h>;
#include "EzShutter.h";
#include "ble.h";

//serial layer
serial_c srl;

void serial_c::init() {
  Serial.begin(9600);
  Serial.setTimeout(100);
}

int serial_c::serial_av() {
  return Serial.available();
}

String serial_c::read_data() {
  return Serial.readStringUntil('\n\r');
}

//Serial out if BLE not connected transfer over BLE
void serial_c::out(char cmd_type, String cmd_val, bool ble) {
  String cmd;
  cmd = String(cmd_type) + DELIMITER + cmd_val;
  if (ble_connected == true && ble == true) {
    ble_tx(cmd);
  } else {
    Serial.availableForWrite();
    Serial.println(cmd);
  }
}