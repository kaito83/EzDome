#ifndef serial_h
#define serial_h
#include <Arduino.h>;

class serial_c {
public:

  int available();
  String read_data();
  void init();
  void out(char cmd_type, String cmd_val);
  void popbuf();
};

extern serial_c srl;
#endif