#ifndef function_h
#define function_h

void fshut_init();
void fshut_reset();
void fshut_move(long pos);
void fshut_estop();
void fshut_monitor_es();
void fshut_actual_es();
void fshut_position();
void fshut_status();
void fshut_transfer_cmd(char cmd_ty, String cmd_val, bool ble, bool trig);
void fshut_emergency_close();
void fshut_init_es();
void fshut_ping();
void fshut_alarm();

#endif
