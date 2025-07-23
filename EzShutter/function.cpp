#include "function.h";
#include "EzShutter.h";
#include "cmd_process.h";
#include "serial.h";
#include "steppers.h";
#include <EEPROM.h>;
//Function layer is responsible for controll everything, like endstop watching motor moving etc..

f_controls ctrls;

stepper stp;

BlockNot bn_ping(15000);
BlockNot bn_alarm(2000);
BlockNot bn_stepper(500);

//reboot function
void (*resetFunc)(void) = 0;

void f_controls::reset() {
  resetFunc();
}

//initing endstops and buttons
void f_controls::init_inputs() {
  pinMode(shutter_rly_pin, OUTPUT);  // init pin for relay

  bes_open.attach(shutter_espin_open, INPUT_PULLUP);
  bes_open.interval(10);
  bes_open.setPressedState(LOW);
  bes_close.attach(shutter_espin_close, INPUT_PULLUP);
  bes_close.interval(10);
  bes_close.setPressedState(LOW);

  btn_open.attach(shutter_bpin_open, INPUT_PULLUP);
  btn_open.interval(10);
  btn_open.setPressedState(LOW);
  btn_close.attach(shutter_bpin_close, INPUT_PULLUP);
  btn_close.interval(10);
  btn_close.setPressedState(LOW);
}

//relay controll, this called before motor start
void f_controls::rly_ctrl(bool on_off) {
  //turn on relay and wait a little bit
  digitalWrite(shutter_rly_pin, on_off);
  delay(50);
}

//alarm if not closed/opened successful or something hitted the endstops
void f_controls::alarm() {
  if (bn_alarm.TRIGGERED && stp.isrun() == false) {
    if (shut_opening == true && shut_es_open == false) {
      srl.out(SHUT_IO_OPEN, "-1", true);
    }
    if (shut_opening == false && shut_es_close == false) {
      srl.out(SHUT_IO_CLOSE, "-1", true);
    }
  }
}

//endstop status chechking, need multiple condition check
bool f_controls::query_es(int es, bool condition) {
  switch (es) {
    //two type of query
    //true if the button was pressed
    //false if the button is currently pressed
    case 0:
      {
        bes_close.update();
        if (condition == true)
          return bes_close.pressed();
        else
          return bes_close.isPressed();
        break;
      }
    case 1:
      {
        bes_open.update();
        if (condition == true)
          return bes_open.pressed();
        else
          return bes_open.isPressed();
        break;
      }
  }
}

String f_controls::pos_in_perc(long pos) {
  return String((double)abs(pos) / (shut_max_move + shut_overlap_move) * 100);
}

//function to move shutter
void f_controls::move(long pos) {
  //prevent to over moves
  if (!stp.isrun()) {
    if (pos > (0 - shut_overlap_move) && shut_es_open) return;               
    if (pos < (shut_max_move + shut_overlap_move) && shut_es_close) return;
  }
  rly_ctrl(1);
  stp.move(pos);
}

//manual buttons for open/close
void f_controls::button_monitoring() {
  btn_close.update();
  btn_open.update();
  if (btn_close.pressed()) {
    shut_opening = false;
    move(0 - shut_overlap_move);
  }
  if (btn_open.pressed()) {
    shut_opening = true;
    move(shut_max_move + shut_overlap_move);
  }
  if (btn_open.isPressed() && btn_close.isPressed()) {
    stp.softstop();
  }
}

//similar to monitor_es without motor stop, used for instant endstop check
void f_controls::actual_es() {
  if (query_es(0, false) == true) {
    srl.out(SHUT_IO_CLOSE, "0", true);
    shut_es_open = false;
    shut_es_close = true;
  } else {
    shut_es_close = false;
    shut_es_open = true;
  }
  if (query_es(1, false) == true) {
    srl.out(SHUT_IO_OPEN, "0", true);
    shut_es_close = false;
    shut_es_open = true;
  } else {
    shut_es_open = false;
    shut_es_close = true;
  }
}

//control function for endstops its runing in main loop, querying endstop status via query_es
//stoping function, instantly stops the motor when endstop reached, and set default position 0 for closed, shut_max_move for open
//recommend calibration process, close the shutter this will be 0
//and try out the max movement becarefull under the process if something goes wrong ready to catch/stop the shutter
//before shut_max_move calibration disable stp.set_positon-s tip add high value for shut_max_move and check where is the position with p#0 command
void f_controls::monitor_es() {
  if (query_es(0, true) == true) {    
    stp.forcestop();
    shut_es_open = false;
    shut_es_close = true;
    rly_ctrl(0);
    stp.set_positon(0);
    srl.out(SHUT_IO_CLOSE, String("1:") + "0", true);
    srl.out(SHUT_IO_CLOSE, "0", true);
  }
  if (query_es(1, true) == true) {    
    stp.forcestop();
    shut_es_close = false;
    shut_es_open = true;
    rly_ctrl(0);
    //disable stp.set_positon during calibration
    stp.set_positon(shut_max_move);
    srl.out(SHUT_IO_OPEN, String("1:") + "100", true);
    srl.out(SHUT_IO_OPEN, "0", true);
  }
}

//function to send ping, I'm here
void f_controls::ping() {
  if (bn_ping.TRIGGERED) {
    srl.out(SHUT_O_PING, "0", true);
  }
}

//moving notification
void f_controls::position() {
  if (stp.isrun() == true && bn_stepper.TRIGGERED) {
    //send val with 1 opening/closeing to report moving with percentage, 0% is closed
    if (shut_opening == false) {
      srl.out(SHUT_IO_CLOSE, String("1:") + pos_in_perc(stp.position()), true);
    } else {
      srl.out(SHUT_IO_OPEN, String("1:") + pos_in_perc(stp.position()), true);
    }
  }
}

//query the position
void f_controls::curr_pos() {
  srl.out(SHUT_IO_QRY_STEPPER_POS, String(stp.position()), true);
  srl.out(SHUT_IO_QRY_STEPPER_POS, String(stp.position()), false); 
}

//shutter emergency stop
void f_controls::estop() {
  stp.forcestop();
  srl.out(SHUT_O_INFORMATION, "Emergency stop!", false);
  srl.out(SHUT_I_EMERGENCY_STOP, "0", true);
  stp.enable(0);
  rly_ctrl(0);
}

//shutter initing
void f_controls::init() {
  srl.out(SHUT_O_INFORMATION, "Shutter initing...", false);
  stp.init(shut_goto_spd, shut_acc);
  //check the shutter clsoed state, if was a power outage or something and couldnt close, then close it, its not controlled by f_controls::move
  if (query_es(0, false) == false) {
    rly_ctrl(1);
    stp.move(-1 * (shut_max_move + shut_overlap_move));
    srl.out(SHUT_O_INFORMATION, "Shutter not closed succesfull, closing", false);
  } else {
    //if evertyhing was normal then setting up internal bools
    shut_opening = false;
    shut_es_open = false;
    shut_es_close = true;
  }
  srl.out(SHUT_O_INFORMATION, version, true);  //sending fw version
}