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
    ctrls.transmit_DDD_ddpos(false);
  }
  ctrls.es_home(ctrls.es_qry(true));
  srl.popbuf();
}
