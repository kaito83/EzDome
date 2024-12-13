#include "EzShutter.h";
#include "cmd_process.h";
#include "serial.h";
#include "steppers.h";
#include "ble.h";
#include <EEPROM.h>;
#include <BlockNot.h>;
#include <Bounce2.h>;


//Function layer is responsible for controll everything, like endstop watching motor moving etc..
Bounce2::Button shut_bes_open = Bounce2::Button();
Bounce2::Button shut_bes_close = Bounce2::Button();

BlockNot fshut_bn_ping(15000);
BlockNot fshut_bn_alarm(2000);
BlockNot fshut_bn_stepper(500);

//reboot function
void (*resetFunc)(void) = 0;
void fshut_reset() {
  resetFunc();
}

//alarm if not closed/opened successful or something hitted the endstops
void fshut_alarm() {
  if (fshut_bn_alarm.TRIGGERED && shut_isrun() == false) {
    if (shut_opening == true && shut_es_open == false) {
      serial_out(SHUT_IO_OPEN, "-1", true);
    }
    if (shut_opening == false && shut_es_close == false) {
      serial_out(SHUT_IO_CLOSE, "-1", true);
    }
  }
}

//function to send ping, I'm here
void fshut_ping() {
  if (fshut_bn_ping.TRIGGERED) {
    serial_out(SHUT_O_PING, "0", true);
  }
}

//initing endstops
void fshut_init_es() {
  shut_bes_open.attach(shutter_espin_open, INPUT_PULLUP);
  shut_bes_open.interval(10);
  shut_bes_open.setPressedState(LOW);
  shut_bes_close.attach(shutter_espin_close, INPUT_PULLUP);
  shut_bes_close.interval(10);
  shut_bes_close.setPressedState(LOW);
}

//moving notification
void fshut_position() {
  if (shut_isrun() == true && fshut_bn_stepper.TRIGGERED) {
    //send val with 1 opening/closeing to report moving is on
    if (shut_opening == false) {
      serial_out(SHUT_IO_CLOSE, "1", true);
    } else {
      serial_out(SHUT_IO_OPEN, "1", true);
    }
  }
}

//function to move shutter
void fshut_move(long pos) {
  shut_init(shut_goto_spd, shut_acc);
  shut_move(pos);
}

//shutter emergency stop
void fshut_estop() {
  shut_stop();
  serial_out(SHUT_O_INFORMATION, "Emergency stop!", false);
  serial_out(SHUT_I_EMERGENCY_STOP, "0", true);
}

//endstop status chechking, need multiple condition check
bool fshut_query_es(int es, bool condition) {
  switch (es) {
    //two type of query
    //true if the button was pressed
    //false if the button is currently pressed
    case 0:
      {
        shut_bes_close.update();
        if (condition == true)
          return shut_bes_close.pressed();
        else
          return shut_bes_close.isPressed();
        break;
      }
    case 1:
      {
        shut_bes_open.update();
        if (condition == true)
          return shut_bes_open.pressed();
        else
          return shut_bes_open.isPressed();
        break;
      }
  }
}

//similar to monitor_es without motor stop, used for instant ES check like
void fshut_actual_es() {
  if (fshut_query_es(0, false) == true) {
    serial_out(SHUT_IO_CLOSE, "0", true);
    shut_es_open = false;
    shut_es_close = true;
  } else {
    shut_es_close = false;
    shut_es_open = true;
  }
  if (fshut_query_es(1, false) == true) {
    serial_out(SHUT_IO_OPEN, "0", true);
    shut_es_close = false;
    shut_es_open = true;
  } else {
    shut_es_open = false;
    shut_es_close = true;
  }
}

//control function for endstops its runing in main loop, querying endstop status via fshut_query_es
//stoping function, instantly stops the motor when endstop reached
void fshut_monitor_es() {
  if (fshut_query_es(0, true) == true) {
    serial_out(SHUT_IO_CLOSE, "0", true);

    shut_stop();
    shut_es_open = false;
    shut_es_close = true;
  }
  if (fshut_query_es(1, true) == true) {
    serial_out(SHUT_IO_OPEN, "0", true);
    shut_stop();
    shut_es_close = false;
    shut_es_open = true;
  }
}

//fast closing
void fshut_emergency_close() {
  shut_init(shut_goto_spd * shut_emergency_mult, shut_acc * shut_emergency_mult);
  fshut_move(0);
}

//shutter initing
void fshut_init() {
  serial_out(SHUT_O_INFORMATION, "Shutter initing...", false);
  //check the shutter clsoed state if was a power outage or something and couldnt close, then close it
  if (fshut_query_es(0, false) == false) {
    shut_init(shut_goto_spd, shut_acc);
    shut_move(-1 * shut_max_move);
    serial_out(SHUT_O_INFORMATION, "Shutter not closed succesfull, closing", false);
  } else {
    //if evertyhing was normal then setting up internal bools
    shut_opening = false;
    shut_es_open = false;
    shut_es_close = true;
  }
  serial_out(SHUT_O_INFORMATION, version, true);  //sending fw version
}