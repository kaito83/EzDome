#include "ble.h";
#include "EzShutter.h";
#include "serial.h";
#include "cmd_process.h";
#include "function.h";
#include <ArduinoBLE.h>;
#include <BlockNot.h>;

//BLE layer

namespace ble {
  BlockNot bn_BLEScan(15000);

  BLECharacteristic RXCharacteristic;
  BLECharacteristic TXCharacteristic(BLE_TXUID, BLEWriteWithoutResponse, 20);
  BLEDevice shutter_peripheral;

  void tx(String cmd) {
    TXCharacteristic.writeValue(cmd.c_str(), false);
  }

  bool rx_upd() {
    return RXCharacteristic.valueUpdated();
  }

  String rx() {
    if (RXCharacteristic) {
      if (RXCharacteristic.valueUpdated()) {
        int rx_length = RXCharacteristic.valueLength();
        byte rx_byte[rx_length + 1];
        RXCharacteristic.readValue(rx_byte, rx_length);
        rx_byte[rx_length] = '\0';
        return String((char*)rx_byte);
      }
    }
    return "";
  }

  void subscribe() {
    srl.out(SHUT_O_INFORMATION, "Connecting to EzDome...", false);
    if (!shutter_peripheral.connect()) {
      srl.out(SHUT_O_INFORMATION, "Can't connect to EzDome, retrying...", false);
      return;
    }

    if (!shutter_peripheral.discoverAttributes()) {
      srl.out(SHUT_O_INFORMATION, "Didn't discover attributes", false);
      shutter_peripheral.disconnect();
      return;
    }

    RXCharacteristic = shutter_peripheral.characteristic(BLE_RXUID);
    TXCharacteristic = shutter_peripheral.characteristic(BLE_TXUID);

    if (RXCharacteristic.canSubscribe() && TXCharacteristic.canSubscribe()) {
      srl.out(SHUT_O_INFORMATION, "EzShutter can subscribe", false);
      if (RXCharacteristic.subscribe() && TXCharacteristic.subscribe()) {
        srl.out(SHUT_O_INFORMATION, "EzShutter subscribed", false);
        ble_connected = true;
      }
    }
  }

  void available() {
    if (!ble_connected && bn_BLEScan.TRIGGERED) {
      shutter_peripheral = BLE.available();
      BLE.scanForUuid(BLE_ROTUID);
      if (shutter_peripheral) {
        if (shutter_peripheral.localName() == BLE_NAME) {
          srl.out(SHUT_O_INFORMATION, String(BLE_NAME) + " found", false);
          BLE.stopScan();
          subscribe();
        }
      }
    }
  }

  void connect(BLEDevice rotator_service) {
    ctrls.actual_es();
    ble_connected = true;
  }

  void disconnected(BLEDevice rotator_service) {
    ble_connected = false;
    srl.out(SHUT_O_INFORMATION, "Disconnected", false);
  }

  void poll() {
    BLE.poll();
  }

  void init() {
    BLE.begin();
    BLE.poll();
    BLE.setEventHandler(BLEDisconnected, disconnected);
    BLE.setEventHandler(BLEConnected, connect);
    BLE.scanForUuid(BLE_ROTUID);
    srl.out(SHUT_O_INFORMATION, "EzRotator scanning for " + String(BLE_NAME), false);
  }
}
