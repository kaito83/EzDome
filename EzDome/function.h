#ifndef function_h
#define function_h

class f_controls {
public:
  bool es_qry(bool condition);
  
  void init();  
  void es_home(bool sw);  
  void reset();
  void transmit_DMpos(bool qry_pos);
  void DMtoSteps(String dm);
  void sync_DM(String dm);
  void cmd_tranfser(String rx);
  void init_inputs();

  void ping();
  void BLE_DC_notif();
  void run_fan(String speed);
  void ALT_limit_check(String ALT);
  void dht_read();
  void rotator_run();
  void rotator_init();
  void rotator_stop();
  void rotator_find_home();
  void rotator_estop();
private:
  void stepToDM(long pos);
  void ALT_flip();
};

extern f_controls ctrls;
#endif
