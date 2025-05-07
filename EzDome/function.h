#ifndef function_h
#define function_h

class f_controls {
public:
  bool es_qry(bool condition);

  void init();
  void es_home(bool sw);
  void reset();
  void transmit_DDD_ddpos(bool qry_pos);
  void DDD_ddtoSteps(float ddd_dd);
  void sync_DDD_dd(float ddd_dd);
  void cmd_tranfser(String rx);
  void init_inputs();

  void ping();
  void BLE_DC_notif();
  void run_fan(String speed);
  void ALT_limit_check(String ALT);
  void dht_read();
  void rotator_init();
  void rotator_stop();
  void rotator_find_home();
  void rotator_estop();
  
  //for testing sitations
  void get_pos();
private:
  String getValue(String data, char separator, int index);
  void stepTo_DDD_dd(long pos);
  void ALT_flip();
};

extern f_controls ctrls;
#endif
