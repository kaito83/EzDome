#include <Arduino.h>;
#include "EzShutter.h";
#include "function.h";
#include "ble.h";

void cmd_process(String rx) {  //INCOMMING CMD PROCESSING
  char cmd_type;
  String cmd_val;
  cmd_type = rx.charAt(0);
  cmd_val = rx.substring(2);

  switch (cmd_type) {
    case SHUT_I_EMERGENCY_CLOSE:
      {
        fshut_emergency_close();
        break;
      }
    case SHUT_I_EMERGENCY_STOP:
      {
        fshut_estop();
        break;
      }
    case SHUT_I_RESET:
      {
        fshut_reset();
        break;
      }
    case SHUT_IO_CLOSE:
      {
        shut_opening = false;
        fshut_move(0);
        break;
      }
    case SHUT_IO_OPEN:
      {
        shut_opening = true;
        fshut_move(shut_max_move);
        break;
      }
    case SHUT_I_QRY_ENDSTOP:
      {
        fshut_actual_es();
        break;
      }
    case SHUT_I_INIT:
      {
        fshut_init();
        break;
      }
  }
}