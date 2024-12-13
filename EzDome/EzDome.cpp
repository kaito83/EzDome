#include <Arduino.h>;
#include <BlockNot.h>;
#include "serial.h";
#include "EzDome.h";
#include "steppers.h";
#include "cmd_process.h";
#include "function.h";
#include "ble.h";
#include "serial.h";

bool rot_swathome = false;
bool rot_zerosearch = false;
bool emergency_stop = false;
bool BLE_connected;

//conversation for DM
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// seting up everything when power is ON
void setup() {
  init_serial();  //init Serial
  frot_init_es();
  rot_init(rot_goto_spd, rot_acc);
  frot_init();
  delay(100);
  ble_init();
  frot_set_fanpins();
}

void loop() {
  frot_ping();
  frot_BLE_DC_notif();

  if (serial_available() > 0) {  //Normal serial communicatino
    String rot_rx_string;
    rot_rx_string = serial_readStringUntil();
    cmd_process(rot_rx_string);
    rot_rx_string = "";
  }

  ble_central_connected();
  if (BLE_connected == true) {
    frot_cmd_tranfser(ble_rx());
  }
  if (emergency_stop == false) {
    rot_run();
    frot_transmit_DMpos(false);
  }
  frot_es_home(frot_es_qry(true));
  ser_popbuf();
}
