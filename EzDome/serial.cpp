#include "serial.h";
#include "EzDome.h";
#include <BlockNot.h>;
#include <FIFObuf.h>;

serial_c srl;
//limiting buffer send rate to 50ms
BlockNot serial_trig(50);
//FIFO to store outgoing commands
FIFObuf<String> ser_fifo(100);

//starting serial communication
void serial_c::init() {
  Serial.begin(9600);
  Serial.setTimeout(100);
}

//check if serial is ready
int serial_c::available() {
  return Serial.available();
}

//reading commands from PC
String serial_c::read_data() {
  return Serial.readStringUntil('\n\r');
}

void serial_c::popbuf() {
  String pop;
  if ((serial_trig.TRIGGERED) && (ser_fifo.size() > 0)) {
    pop = ser_fifo.pop();
    if (pop != "") {
      Serial.availableForWrite();
      Serial.println(pop);
    }
  }
}

void serial_c::out(char cmd_type, String cmd_val) {
  ser_fifo.push(String(cmd_type) + DELIMITER + cmd_val);
}