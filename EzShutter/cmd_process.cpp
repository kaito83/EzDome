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
        ctrls.rly_ctrl(1);
      //  ctrls.emergency_close();
        break;
      }
    case SHUT_I_EMERGENCY_STOP:
      {        
        ctrls.estop();
        break;
      }
    case SHUT_I_RESET:
      {
        ctrls.reset();
        break;
      }
    case SHUT_IO_CLOSE:
      {
        shut_opening = false;
        ctrls.rly_ctrl(1);
        ctrls.move(0 - shut_overlap_move);
        break;
      }
    case SHUT_IO_OPEN:
      {
        shut_opening = true;
        ctrls.rly_ctrl(1);
        ctrls.move(shut_max_move + shut_overlap_move);
        break;
      }
    case SHUT_I_QRY_ENDSTOP:
      {
        ctrls.actual_es();
        break;
      }
    case SHUT_IO_QRY_STEPPER_POS:
      {
        ctrls.curr_pos();
        break;
      }
    case SHUT_I_INIT:
      {
        ctrls.init();
        break;
      }
  }
}