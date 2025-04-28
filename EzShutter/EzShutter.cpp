#include <Arduino.h>;
#include "serial.h";
#include "EzShutter.h";
#include "cmd_process.h";
#include "function.h";
#include "ble.h";

bool shut_es_open = false;
bool shut_es_close = false;

bool emergency_stop = false;
bool ble_connected = false;
bool shut_opening;
long shut_open_max;

void setup() {
  shut_open_max = shut_max_move;  // set the maximum opening to the real opening position
  srl.init();                     //init Serial
  ctrls.init_inputs();            // init endstops
  delay(100);
  ctrls.init();  //init shutter stuff
  ble_init();    // init BLE
  delay(100);
  ble_available();
  delay(100);
  ble_subscribe();
}

void loop() {
  ctrls.ping();
  ctrls.monitor_es();
  ctrls.button_monitoring();
  if (emergency_stop == false) {
    ctrls.position();
  }
  ctrls.alarm();
  
  //Incoming BLE commands just transfering to processing layer
  ble_available();  //need in the main loop
  if (ble_connected == true) {
    if (ble_rx_upd() == true) {
      cmd_process(ble_rx());
    }
  }
  //
  //normal serial reading for USB testing and etc.
  if (srl.serial_av() > 0) {
    String shut_rx_string;
    shut_rx_string = srl.read_data();
    cmd_process(shut_rx_string);
    shut_rx_string = "";
  }
}