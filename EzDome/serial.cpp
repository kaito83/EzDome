#include <Arduino.h>;
#include "EzDome.h";
#include <BlockNot.h>;
#include <FIFObuf.h>;

//limiting buffer send rate to 50ms
BlockNot serial_trig(50);
//FIFO to store commands
FIFObuf<String> ser_fifo(30);

//starting serial communication
void init_serial() {
  Serial.begin(9600);
  Serial.setTimeout(100);
}

//check if serial is ready
int serial_available() {
  return Serial.available();
}

//reading commands from PC
String serial_readStringUntil() {
  return Serial.readStringUntil('\n\r');
}

void ser_popbuf() {
  String pop;
  if ((serial_trig.TRIGGERED) && (ser_fifo.size() > 0)) {
    pop = ser_fifo.pop();
    if (pop != "") {
      Serial.availableForWrite();
      Serial.println(pop);
    }
  }
}

void serial_out(char cmd_type, String cmd_val) {
  ser_fifo.push(String(cmd_type) + DELIMITER + cmd_val);
}