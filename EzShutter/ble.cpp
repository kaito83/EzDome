
#include "EzShutter.h";
#include "serial.h";
#include "cmd_process.h";
#include "function.h";
#include <Arduino.h>;
#include <ArduinoBLE.h>;

BLECharacteristic RXCharacteristic;
BLECharacteristic TXCharacteristic;
BLEDevice shutter_peripheral;

//transfer msg to rotator
void ble_tx(String cmd) {
  TXCharacteristic.writeValue(cmd.c_str());
}

bool ble_rx_upd() {
  return RXCharacteristic.valueUpdated();
}

String ble_rx() {
  if (RXCharacteristic) {
    if (RXCharacteristic.valueUpdated()) {
      int rx_length = RXCharacteristic.valueLength();
      byte rx_byte[rx_length + 1];
      RXCharacteristic.readValue(rx_byte, rx_length);
      rx_byte[rx_length] = '\0';
      return String((char*)rx_byte);
    }
  }
}

//Subscribe for the characteristics
void ble_subscribe() {
  srl.out(SHUT_O_INFORMATION, "Connecting to EzDome...", false);
  if (!shutter_peripheral.connect()) {
    srl.out(SHUT_O_INFORMATION, "Can't connect to EzDome, retrying...", false);
  }
  if (!shutter_peripheral.discoverAttributes()) {
    srl.out(SHUT_O_INFORMATION, "Didn't discover attributes", false);
    shutter_peripheral.disconnect();
  }
  RXCharacteristic = shutter_peripheral.characteristic(BLE_RXUID);
  TXCharacteristic = shutter_peripheral.characteristic(BLE_TXUID);
  if (RXCharacteristic.canSubscribe() && TXCharacteristic.canSubscribe()) {
    srl.out(SHUT_O_INFORMATION, "EzShutter can subscribe", false);
    ble_connected = false;
    if (RXCharacteristic.subscribe() && TXCharacteristic.subscribe()) {
      srl.out(SHUT_O_INFORMATION, "EzShutter subscribed", false);
      ble_connected = true;
    }
  }
}

//Scaning for shutter
void ble_available() {
  shutter_peripheral = BLE.available();
  if (shutter_peripheral) {
    if (shutter_peripheral.localName() == BLE_NAME) {
      srl.out(SHUT_O_INFORMATION, String(BLE_NAME) + " found", false);
      BLE.stopScan();
      ble_subscribe();
    }
    BLE.scanForUuid(BLE_ROTUID);
  }
}

void ble_connect(BLEDevice rotator_service) {
  //if BLE connected send actual status of shutter
  ctrls.actual_es();
  ble_connected = true;
}


void ble_disconnected(BLEDevice rotator_service) {
  ble_connected = false;
  srl.out(SHUT_O_INFORMATION, "Disconnected", false);
}

//Initing and starting BLE device
void ble_init() {
  BLE.begin();
  BLE.poll();
  BLE.setEventHandler(BLEDisconnected, ble_disconnected);
  BLE.setEventHandler(BLEConnected, ble_connect);
  BLE.scanForUuid(BLE_ROTUID);
  srl.out(SHUT_O_INFORMATION, "EzRotator scaning for " + String(BLE_NAME), false);
}
