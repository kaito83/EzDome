#include <AccelStepper.h>;
#include "steppers.h";
//#include <Wire.h>;
#include "EzDome.h";

//Stepper layer is responsible for the motor controlling

AccelStepper rotator(AccelStepper::DRIVER, rotator_stp, rotator_dir);

//initing motor
void stepper::init(float speed, float accel) {
  rotator.setEnablePin(rotator_en);
  rotator.setMaxSpeed(speed);
  rotator.setAcceleration(accel);
  rotator.setEnablePin(rotator_en);
  rotator.setPinsInverted(rotator_dir_invert, rotator_stp_invert, rotator_en_invert);
  rotator.enableOutputs();
}

// rotor movement
/*long stepper::move(long move) {
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
*/

long stepper::move(long move) {
  long current = rotator.currentPosition();
  long delta = move - current;

  // Normalizáljuk az elmozdulást a -180 és +180 fok közé
  if (delta > rot_full_rotation / 2) {
    delta -= rot_full_rotation;
  } else if (delta < -rot_full_rotation / 2) {
    delta += rot_full_rotation;
  }

  long target = current + delta;
  rotator.moveTo(target);
  return delta;
}

//this must be in main loop, to run motor
void stepper::run() {
  rotator.run();
}

//stoping the motor
void stepper::stop() {
  unsigned long startMillis;
  unsigned long currentMillis;
  rotator.stop();
  currentMillis = millis();
  if (currentMillis - startMillis >= 1) {
    startMillis = currentMillis;
  }
}

//disable motors enable pin
void stepper::disable(bool S) {
  digitalWrite(rotator_en, S);
  rotator.disableOutputs();
  rotator.setCurrentPosition(rotator.currentPosition());
}

//updating motor position
void stepper::set_positon(long pos) {
  rotator.setCurrentPosition(pos);
}

//query motor position
long stepper::position() {
  long pos;
  pos = rotator.currentPosition();
  if (pos >= rot_full_rotation)
    pos -= rot_full_rotation;
  if (pos < 0)
    pos += rot_full_rotation;
  return pos;
}

//check motor is runing
bool stepper::isrun() {
  bool run;
  run = rotator.isRunning();
  return run;
}
