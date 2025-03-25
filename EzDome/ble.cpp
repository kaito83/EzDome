#include <Arduino.h>;
#include "EzDome.h";
#include "function.h";
#include "serial.h";
#include <ArduinoBLE.h>;

String prev_rx;

BLEService rotator_service(BLE_ROTUID);
BLEStringCharacteristic TXCharacteristic(BLE_TXUID, BLEWrite | BLERead | BLENotify, 20);
BLEStringCharacteristic RXCharacteristic(BLE_RXUID, BLEWrite | BLERead | BLENotify, 20);

void ble_tx(char cmd_type, String cmd_val) {
  String tx_cmd;
  if (BLE_connected == true) {
    tx_cmd = String(cmd_type) + DELIMITER + cmd_val;
    TXCharacteristic.writeValue(tx_cmd);
  }
}

String ble_rx() {
  String val_rx;
  if (RXCharacteristic.written() == true) {
    val_rx = RXCharacteristic.value();
    if (prev_rx != val_rx)
      return val_rx;
  }
  prev_rx = val_rx;
  return "";
}

void ble_disconnected(BLEDevice rotator_service) {
  BLE_connected = false;
  srl.out(SHUT_O_INFORMATION, "0");
}

void ble_connected(BLEDevice rotator_service) {
  BLE_connected = true;
  ble_tx(SHUT_I_QRY_ENDSTOP, "0");
  srl.out(SHUT_O_INFORMATION, "1");
}

void ble_init() {
  BLE.begin();
  BLE.poll();
  BLE.setEventHandler(BLEDisconnected, ble_disconnected);
  BLE.setEventHandler(BLEConnected, ble_connected);
  BLE.setLocalName(BLE_NAME);
  BLE.setAdvertisedService(rotator_service);
  rotator_service.addCharacteristic(TXCharacteristic);
  rotator_service.addCharacteristic(RXCharacteristic);
  BLE.addService(rotator_service);
  TXCharacteristic.writeValue("0");
  RXCharacteristic.writeValue("0");
  BLE.advertise();
  srl.out(SHUT_O_INFORMATION, "2");
}

void ble_central_connected() {
  BLEDevice central = BLE.central();
  if (central) {
    if (central.connected()) {
    }
  }
}
