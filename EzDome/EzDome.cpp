#include "serial.h";
#include "EzDome.h";
#include "steppers.h";
#include "cmd_process.h";
#include "function.h";
#include "ble.h";
#include <Arduino.h>;
#include <BlockNot.h>;

bool rot_swathome = false;
bool rot_zerosearch = false;
bool emergency_stop = false;
bool BLE_connected;
bool rot_ignore_AZ = false;

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
  srl.init();  //init Serial
  ctrls.init_inputs();
  ctrls.rotator_init();
  delay(100);
  ble_init();
}

void loop() {
  ctrls.ping();
  ctrls.dht_read();
  ctrls.BLE_DC_notif();

  if (srl.available() > 0) {  //Normal serial communication
    String rot_rx_string;
    rot_rx_string = srl.read_data();
    cmd_process(rot_rx_string);
    rot_rx_string = "";
  }

  ble_central_connected();
  if (BLE_connected == true) {
    ctrls.cmd_tranfser(ble_rx());
  }
  if (emergency_stop == false) {
    ctrls.rotator_run();
    ctrls.transmit_DMpos(false);
  }
  ctrls.es_home(ctrls.es_qry(true));
  srl.popbuf();
}
