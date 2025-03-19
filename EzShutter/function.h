#ifndef function_h
#define function_h

#include <BlockNot.h>;
#include <Bounce2.h>;

class f_controls {
private:
  Bounce2::Button bes_open = Bounce2::Button();
  Bounce2::Button bes_close = Bounce2::Button();
  Bounce2::Button btn_open = Bounce2::Button();
  Bounce2::Button btn_close = Bounce2::Button();
public:
  void init();
  void init_inputs();
  void reset();
  void move(long pos);
  void position();
  void curr_pos();
  void estop();
  void emergency_close();  
  void monitor_es();
  void actual_es();
  void button_monitoring();
  void alarm();
  void rly_ctrl(int on_off);
  void ping();
  bool stepper_run();
private:
  bool query_es(int es, bool condition);
};

extern f_controls ctrls;
#endif
