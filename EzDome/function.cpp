#include "EzDome.h";
#include "steppers.h";
#include "function.h";
#include "ble.h";
#include "serial.h";
#include <BlockNot.h>;
#include <Bounce2.h>;
#include <math.h>;
#include <DHT.h>;


f_controls ctrls;

stepper stp;
//Function layer is responsible for controll everything, like calc steps to DM, checking endstop etc.

BlockNot frot_bn_ping(15000);
//for BLE active send status 5sec connected/disconnect
BlockNot frot_bn_BLE(5000);
//DHT reading 1min you can change this
BlockNot frot_bn_dht(long(rotator_dht_read));
//sending the stepper position frequency is calculated, to reduce load
BlockNot fshut_bn_stepper(round(rot_full_rotation / rot_goto_spd) - (round(rot_full_rotation / rot_goto_spd) / 10));

//endstop check with bounce
Bounce2::Button shut_bes_home = Bounce2::Button();

//humidity and temp sensor
DHT dht(rotator_dht_pin, rot_dht_type);


String DMSEP = ":";
char DMSEP_C = ':';

//Soft reseting the board
void (*resetFunc)(void) = 0;

//Restarting hardware
void f_controls::reset() {
  resetFunc();
}

void f_controls::dht_read() {
  if (frot_bn_dht.TRIGGERED) {
    srl.out(ROT_DHT, String(dht.readTemperature()) + DMSEP + String(dht.readHumidity()));
  }
}

// fan section


void f_controls::run_fan(String speed) {
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
void f_controls::BLE_DC_notif() {
  if (frot_bn_BLE.TRIGGERED)
    if (BLE_connected == true)
      srl.out(SHUT_O_INFORMATION, "1");
    else
      srl.out(SHUT_O_INFORMATION, "0");
}

// initing endstop and other sensors
void f_controls::init_inputs() {
  shut_bes_home.attach(rotator_es_home, INPUT_PULLUP);
  shut_bes_home.interval(10);
  shut_bes_home.setPressedState(HIGH);
  dht.begin();
  pinMode(fan0_PWM, OUTPUT);
  pinMode(fan0_pin0, OUTPUT);
  pinMode(fan0_pin1, OUTPUT);
}

//function to send ping, I'm here
void f_controls::ping() {
  if (frot_bn_ping.TRIGGERED) {
    srl.out(ROT_O_PING, "0");
  }
}

//Endstop reading
bool f_controls::es_qry(bool condition) {
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
void f_controls::rotator_run() {
  stp.run();
}

void f_controls::rotator_init() {
  stp.init(rot_goto_spd, rot_acc);
}

//always called to check home position
void f_controls::es_home(bool es_qry) {
  if (es_qry == true) {
    srl.out(ROT_O_INFORMATION, "3");
  }
  if (es_qry == true && rot_zerosearch == true) {
    rot_zerosearch = false;
    stp.set_positon(0);
    srl.out(ROT_IO_DMPOS, String(0));
    srl.out(ROT_O_INFORMATION, "4");
  }
}

//converting steps to DM
void f_controls::stepToDM(long pos) {
  String s_pos;
  double mm = 0.000;
  double fr;
  pos = stp.position();
  mm = pos / (rot_step_DM);
  if (mm < 0.000)
    mm += 21600.000;
  if (mm >= 21600.000)
    mm -= 21600.000;
  s_pos = String(int(trunc(mm / 60))) + DMSEP + String(int(trunc(modf(mm / 60, &fr) * 60)));
  srl.out(ROT_IO_DMPOS, s_pos);
}

//transmitting rotator position
void f_controls::transmit_DMpos(bool qry_pos) {
  //transmit if runing with triggered calc
  if (stp.isrun() == true && fshut_bn_stepper.TRIGGERED) {
    stepToDM(stp.position());
  }
  //used for quering the position
  if (qry_pos == true) {
    stepToDM(stp.position());
  }
}

//syncing rotator to mount
void f_controls::sync_DM(String dm) {
  int ddd;
  int mm;
  ddd = getValue(dm, DMSEP_C, 0).toInt();
  mm = getValue(dm, DMSEP_C, 1).toInt();
  stp.set_positon((60 * ddd * rot_step_DM) + (mm * rot_step_DM));
  ctrls.transmit_DMpos(true);
}

// rotator 180° flipping function
void f_controls::ALT_flip() {
  if (rot_ignore_AZ == false) {
    if (rot_full_rotation / 2 > stp.position()) {
      stp.move(stp.position() + (rot_full_rotation / 2));
    } else {
      stp.move(stp.position() - (rot_full_rotation / 2));
    }
    srl.out(ROT_O_INFORMATION, "5");
  }
}

//disable AZ coordinates until the ALT lower than rot_max_ALT, called from cmd_proccess
void f_controls::ALT_limit_check(String ALT) {
  if (ALT.toInt() >= rot_max_ALT) {
    //flip called once, and set rot_ignore_AZ to true
    ALT_flip();
    rot_ignore_AZ = true;
  }
  if (ALT.toInt() < rot_max_ALT && rot_ignore_AZ == true) {
    rot_ignore_AZ = false;
    srl.out(ROT_O_INFORMATION, "6");
  }
}

//converting DM to steps
void f_controls::DMtoSteps(String dm) {
  int ddd;
  int mm;
  ddd = getValue(dm, DMSEP_C, 0).toInt();
  mm = getValue(dm, DMSEP_C, 1).toInt();
  stp.move((60 * ddd * rot_step_DM) + (mm * rot_step_DM));
}

//emergency stop function instantly stop everything
void f_controls::rotator_estop() {
  emergency_stop = true;
  stp.stop();
  stp.disable(true);
  srl.out(ROT_O_INFORMATION, "0");
}

//rotator stoping
void f_controls::rotator_stop() {
  stp.stop();
  srl.out(ROT_O_INFORMATION, "1");
}

//used for homing
void f_controls::rotator_find_home() {
  if (rot_swathome == false) {
    rot_zerosearch = true;
    stp.init(rot_goto_spd, rot_acc);
    stp.move(rot_full_rotation * find_home_overlap_percent);
    srl.out(ROT_O_INFORMATION, "2");
  }
}

//rotator initing send info about params resolution, position, and status
void f_controls::init() {
  srl.out(ROT_O_PARAMS, "0" + DMSEP + version);
  srl.out(ROT_O_PARAMS, "1" + DMSEP + String(rot_step_rev));
  srl.out(ROT_O_PARAMS, "2" + DMSEP + String(rot_full_rotation));
  srl.out(ROT_O_PARAMS, "3" + DMSEP + String(rot_step_DM));
  srl.out(ROT_O_PARAMS, "4" + DMSEP + String(stp.position()));
  //check home status
  es_home(es_qry(false));
  //check position
  transmit_DMpos(true);
  //query shutter's es status
  ble_tx(SHUT_I_QRY_ENDSTOP, "0");
}

//transmitting commands from shutter hardware to rotator serial
void f_controls::cmd_tranfser(String rx) {
  if (rx != "") {
    char cmd_type;
    String cmd_val;
    cmd_type = rx.charAt(0);
    cmd_val = rx.substring(2);
    srl.out(cmd_type, cmd_val);
  }
}