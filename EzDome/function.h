#ifndef function_h
#define function_h

void frot_estop();
void frot_init();
void frot_find_home();
void frot_es_home(bool sw);
void frot_stop();
void frot_reset();
void frot_transmit_DMpos(bool qry_pos);
void frot_DMtoSteps(String dm);
void frot_sync_DM(String dm);
void frot_cmd_tranfser(String rx);
void frot_init_es();
bool frot_es_qry(bool condition);
void frot_ping();
void frot_BLE_DC_notif();
void frot_set_fanpins();
void frot_run_fan(String speed);

#endif
