#include <Arduino.h>;
#include "EzShutter.h";
#include "ble.h";
#include <FIFObuf.h>;
#include <BlockNot.h>;


//serial layer

FIFObuf<String> ser_fifo(30);
BlockNot serial_trig(30);

void init_serial() {
  Serial.begin(9600);
  Serial.setTimeout(100);
}

int serial_available() {
  return Serial.available();
}

String serial_readStringUntil() {
  return Serial.readStringUntil('\n\r');
}

//Serial out if BLE not connected transfer over BLE
void serial_out(char cmd_type, String cmd_val, bool ble) {
  String cmd;
  cmd = String(cmd_type) + DELIMITER + cmd_val;
  if (ble_connected == true && ble == true)
    ble_tx(cmd);
  else {
    Serial.availableForWrite();
    Serial.println(cmd);
  }
}