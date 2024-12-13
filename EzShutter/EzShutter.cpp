#include <Arduino.h>;
#include "serial.h";
#include "EzShutter.h";
#include "steppers.h";
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
  init_serial();                  //init Serial
  fshut_init_es();                // init endstops
  delay(100);
  fshut_init();  //init shutter stuff
  ble_init();    // init BLE
  delay(100);
  ble_available();
  delay(100);
  ble_subscribe();
}

void loop() {
  fshut_ping();
  fshut_monitor_es(); 
  //Incoming BLE commands just transfering to processing layer
  ble_available();  //need to in the main loop
  if (ble_connected == true) {
    if (ble_rx_upd() == true) {
      cmd_process(ble_rx());
    }
  }
  //
  //normal serial reading for USB testing and etc.
  if (serial_available() > 0) {
    String shut_rx_string;
    shut_rx_string = Serial.readStringUntil('\n\r');
    cmd_process(shut_rx_string);
    shut_rx_string = "";
  }

  if (emergency_stop == false) {
    shut_run();
    fshut_position();
  }
  fshut_alarm();
}