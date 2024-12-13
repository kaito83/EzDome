#include "EzDome.h";
#include "steppers.h";
#include <math.h>;
#include "serial.h";
#include <BlockNot.h>;
#include <Bounce2.h>;
#include <ble.h>;

//Function layer is responsible for controll everything, like calc steps to DM, checking endstop etc.

BlockNot frot_bn_ping(15000);
//for BLE active send status 5sec connected/disconnect
BlockNot frot_bn_BLE(5000);
//sending the stepper position frequency is calculated, to reduce load
BlockNot fshut_bn_stepper(round(rot_full_rotation / rot_goto_spd) - (round(rot_full_rotation / rot_goto_spd) / 10));

Bounce2::Button shut_bes_home = Bounce2::Button();

String DMSEP = ":";
char DMSEP_C = ':';

//Soft reseting the board
void (*resetFunc)(void) = 0;

//Restarting hardware
void frot_reset() {
  resetFunc();
}

// fan section
void frot_set_fanpins() {
  //fan 0
  pinMode(fan0_PWM, OUTPUT);
  pinMode(fan0_pin0, OUTPUT);
  pinMode(fan0_pin1, OUTPUT);
  //ledcAttach(fan0_PWM, 500, 8);

  //fan 1
 // pinMode(fan1_PWM, OUTPUT);
 // pinMode(fan1_pin0, OUTPUT);
 // pinMode(fan1_pin1, OUTPUT);
}

void frot_run_fan(String speed) {
  // int fanid = getValue(c, DMSEP_C, 0).toInt();
  // int speed = getValue(c, DMSEP_C, 1).toInt();
  // switch (fanid) {
  //   case 0:
  //     {
  //       if (speed <= 1) {
  //         digitalWrite(fan0_pin0, LOW);
  //         digitalWrite(fan0_pin1, LOW);
  //       } else {
  //         //switch if you want to swap spin directino HIGH<>LOW
  //         digitalWrite(fan0_pin0, HIGH);
  //         digitalWrite(fan0_pin1, LOW);
  //        // ledcWrite(fan0_PWM,speed);
  //         analogWrite(fan0_PWM, speed);
  //       }
  //       break;
  //     }
  //   case 1:
  //     {
  //       if (speed  <= 1) {
  //         digitalWrite(fan1_pin0, LOW);
  //         digitalWrite(fan1_pin1, LOW);
  //       } else {
  //         //switch if you want to swap spin directino HIGH<>LOW
  //         digitalWrite(fan1_pin0, HIGH);
  //         digitalWrite(fan1_pin1, LOW);
  //         analogWrite(fan1_PWM, speed);
  //       }
  //       break;
  //     }
  // }


  if (speed.toInt() <= 1) {
    digitalWrite(fan0_pin0, LOW);
    digitalWrite(fan0_pin1, LOW);
  } else {
    //switch if you want to swap spin directino HIGH<>LOW if motor supported, other wise 
    digitalWrite(fan0_pin0, HIGH);
    digitalWrite(fan0_pin1, LOW);
    analogWrite(fan0_PWM, speed.toInt());
  }
}

//if shutter cannot connected send rapidly the message
void frot_BLE_DC_notif() {
  if (frot_bn_BLE.TRIGGERED)
    if (BLE_connected == true)
      serial_out(SHUT_O_INFORMATION, "1");
    else
      serial_out(SHUT_O_INFORMATION, "0");
}

// initing endstop
void frot_init_es() {
  shut_bes_home.attach(rotator_es_home, INPUT_PULLUP);
  shut_bes_home.interval(10);
  shut_bes_home.setPressedState(HIGH);
}

//function to send ping, I'm here
void frot_ping() {
  if (frot_bn_ping.TRIGGERED) {
    serial_out(ROT_O_PING, "0");
  }
}

//Endstop reading
bool frot_es_qry(bool condition) {
  //two type of condition
  //true if the button was pressed
  //false if the button is currently pressed
  shut_bes_home.update();
  if (condition == true) {
    return shut_bes_home.pressed();
  } else {
    return shut_bes_home.isPressed();
  }
}

//always called to check home position
void frot_es_home(bool es_qry) {
  if (es_qry == true) {
    serial_out(ROT_O_INFORMATION, "3");
  }
  if (es_qry == true && rot_zerosearch == true) {
    rot_zerosearch = false;
    rot_set_positon(0);
    serial_out(ROT_IO_DMPOS, String(0));
    serial_out(ROT_O_INFORMATION, "4");
  }
}

//converting steps to DM
void frot_stepToDM(long pos) {
  String s_pos;
  double mm = 0.000;
  double fr;
  pos = rot_position();
  mm = pos / (rot_step_DM);
  if (mm < 0.000)
    mm += 21600.000;
  if (mm >= 21600.000)
    mm -= 21600.000;
  s_pos = String(int(trunc(mm / 60))) + DMSEP + String(int(trunc(modf(mm / 60, &fr) * 60)));
  serial_out(ROT_IO_DMPOS, s_pos);
}

//transmitting rotator position
void frot_transmit_DMpos(bool qry_pos) {
  //transmit if runing with triggered calc
  if (rot_isrun() == true && fshut_bn_stepper.TRIGGERED) {
    frot_stepToDM(rot_position());
  }
  //used for quering the position
  if (qry_pos == true) {
    frot_stepToDM(rot_position());
  }
}

//syncing rotator to mount
void frot_sync_DM(String dm) {
  int ddd;
  int mm;
  ddd = getValue(dm, DMSEP_C, 0).toInt();
  mm = getValue(dm, DMSEP_C, 1).toInt();
  rot_set_positon((60 * ddd * rot_step_DM) + (mm * rot_step_DM));
  frot_transmit_DMpos(true);
}

//converting DM to steps
void frot_DMtoSteps(String dm) {
  int ddd;
  int mm;
  ddd = getValue(dm, DMSEP_C, 0).toInt();
  mm = getValue(dm, DMSEP_C, 1).toInt();
  rot_move((60 * ddd * rot_step_DM) + (mm * rot_step_DM));
}

//emergency stop function instantly stop everything
void frot_estop() {
  emergency_stop = true;
  rot_stop();
  rot_disable(true);
  serial_out(ROT_O_INFORMATION, "0");
}

//rotator stoping
void frot_stop() {
  rot_stop();
  serial_out(ROT_O_INFORMATION, "1");
}

//used for homing
void frot_find_home() {
  if (rot_swathome == false) {
    rot_zerosearch = true;
    rot_init(rot_goto_spd, rot_acc);
    rot_move(rot_full_rotation * find_home_overlap_percent);
    serial_out(ROT_O_INFORMATION, "2");
  }
}

//rotator initing send info about params resolution, position, and status
void frot_init() {
  serial_out(ROT_O_PARAMS, "0" + DMSEP + version);
  serial_out(ROT_O_PARAMS, "1" + DMSEP + String(rot_step_rev));
  serial_out(ROT_O_PARAMS, "2" + DMSEP + String(rot_full_rotation));
  serial_out(ROT_O_PARAMS, "3" + DMSEP + String(rot_step_DM));
  serial_out(ROT_O_PARAMS, "4" + DMSEP + String(rot_position()));
  //check home status
  frot_es_home(frot_es_qry(false));
  //check position
  frot_transmit_DMpos(true);
  //query shutter's es status
  ble_tx(SHUT_I_QRY_ENDSTOP, "0");
}

//transmitting commands from shutter hardware to rotator serial
void frot_cmd_tranfser(String rx) {
  if (rx != "") {
    char cmd_type;
    String cmd_val;
    cmd_type = rx.charAt(0);
    cmd_val = rx.substring(2);
    serial_out(cmd_type, cmd_val);
  }
}