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
        frot_estop();
        break;
      }
    case ROT_I_RESET:
      {
        ble_tx(SHUT_I_RESET, cmd_val);
        frot_reset();
        break;
      }
    case ROT_I_STOP:
      {
        frot_stop();
        break;
      }
    case ROT_IO_DMPOS:
      {
        frot_DMtoSteps(cmd_val);
        break;
      }
    case ROT_I_QUERY_POS:
      {
        frot_transmit_DMpos(true);
        break;
      }
    case ROT_I_SYNC:
      {
        frot_sync_DM(cmd_val);
        break;
      }
    case ROT_IO_HOME:
      {
        frot_find_home();
        break;
      }
    case ROT_I_REQUEST_INIT:
      {
        frot_init();
        break;
      }
    case ROT_I_FAN:
    {
      frot_run_fan(cmd_val);
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
        frot_es_home(frot_es_qry(false));
        break;
      }
  }
}
