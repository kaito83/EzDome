#include <AccelStepper.h>;
//#include <Wire.h>;
#include "EzDome.h";

//Stepper layer is responsible for the motor controlling

AccelStepper rotator(AccelStepper::DRIVER, rotator_stp, rotator_dir);

//initing motor
void rot_init(float speed, float accel) {
  rotator.setEnablePin(rotator_en);
  rotator.setMaxSpeed(speed);
  rotator.setAcceleration(accel);
  rotator.setEnablePin(rotator_en);
  rotator.setPinsInverted(rotator_dir_invert, rotator_stp_invert, rotator_en_invert);
  rotator.enableOutputs();
}

// rotor movement
long rot_move(long move) {
  long p;
  //always find the shortest route
  if (rotator.currentPosition() + rot_full_rotation / 2 <= move && rot_zerosearch == false) {

    p = (rot_full_rotation - move) * -1;
    rotator.moveTo(p);
    return p;
  } else {
    rotator.moveTo(move);
    return move;
  }
}

//this must be in main loop, to run motor
void rot_run() {
  rotator.run();
}

//stoping the motor
void rot_stop() {
  unsigned long startMillis;
  unsigned long currentMillis;
  rotator.stop();
  currentMillis = millis();
  if (currentMillis - startMillis >= 1) {
    startMillis = currentMillis;
  }
}

//disable motors enable pin
void rot_disable(bool S) {
  digitalWrite(rotator_en, S);
  rotator.disableOutputs();
  rotator.setCurrentPosition(rotator.currentPosition());
}

//updating motor position
void rot_set_positon(long pos) {
  rotator.setCurrentPosition(pos);
}

//query motor position
long rot_position() {
  long pos;
  pos = rotator.currentPosition();
  if (pos >= rot_full_rotation)
    pos -= rot_full_rotation;
  if (pos < 0)
    pos += rot_full_rotation;
  return pos;
}

//check motor is runing
bool rot_isrun() {
  bool run;
  run = rotator.isRunning();
  return run;
}
