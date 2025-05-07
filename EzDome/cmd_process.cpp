#include "EzDome.h";
#include "function.h";
#include "ble.h";

//cmd_process layer is responsible for calling the right functions

void cmd_process(String rx) {  //Incoming serial commands from pc
  char cmd_type;
  String cmd_val;
  cmd_type = rx.charAt(0);
  cmd_val = rx.substring(2);

  switch (cmd_type) {
    case ROT_I_EMERGENCY_STOP:
      {
        ctrls.rotator_estop();
        break;
      }
    case ROT_I_RESET:
      {
        ble_tx(SHUT_I_RESET, cmd_val);
        ctrls.reset();
        break;
      }
    case ROT_I_STOP:
      {
        ctrls.rotator_stop();
        break;
      }
    case ROT_I_ALT:
      {
        ctrls.ALT_limit_check(cmd_val);
        break;
      }
    case ROT_IO_DDDPOS:
      {
        if (rot_ignore_AZ == false) {
          ctrls.DDD_ddtoSteps(cmd_val.toDouble());
        }
        break;
      }
    case ROT_I_QUERY_POS:
      {
        ctrls.transmit_DDD_ddpos(true);
        break;
      }
    case ROT_I_SYNC:
      {
        ctrls.sync_DDD_dd(cmd_val.toDouble());
        break;
      }
    case ROT_IO_HOME:
      {
        ctrls.rotator_find_home();
        break;
      }
    case ROT_I_REQUEST_INIT:
      {
        ctrls.init();
        break;
      }
    case ROT_I_FAN:
      {
        ctrls.run_fan(cmd_val);
        break;
      }
    // shutter commands just transfering over BLE
    case SHUT_I_QRY_ENDSTOP:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    case SHUT_I_INIT:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    case SHUT_I_EMERGENCY_STOP:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    case SHUT_I_EMERGENCY_CLOSE:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    case SHUT_I_RESET:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    case SHUT_IO_CLOSE:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    case SHUT_IO_OPEN:
      {
        ble_tx(cmd_type, cmd_val);
        break;
      }
    // for testing,debuging etc just for development
    case C_TEST:
      {
        ctrls.get_pos();
        break;
      }
  }
}
