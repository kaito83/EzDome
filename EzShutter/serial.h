#ifndef serial_h
#define serial_h
#include <Arduino.h>;
//#include <FIFObuf.h>;
//#include <BlockNot.h>;

class serial_c {
public:
  int serial_av();
  String read_data();
  void init();
  void out(char cmd_type, String cmd_val, bool ble);
};

extern serial_c srl;

#endif