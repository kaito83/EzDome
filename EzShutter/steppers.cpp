#include "steppers.h";
#include "EzShutter.h";
#include <AccelStepper.h>;
#include <Wire.h>;

AccelStepper shutter(AccelStepper::DRIVER, shutter_stp, shutter_dir);

void stepper::init(float speed, float accel) {
  shutter.setEnablePin(shutter_en);
  shutter.setMaxSpeed(speed);
  shutter.setAcceleration(accel);
  shutter.setEnablePin(shutter_en);
  shutter.setPinsInverted(shutter_dir_invert, shutter_stp_invert, shutter_en_invert);
  shutter.enableOutputs();
}

void stepper::move(long move) {
  shutter.moveTo(move);
}

void stepper::run() {
  shutter.run();
}

long stepper::position() {
  return shutter.currentPosition();
}

void stepper::stop() {
  unsigned long startMillis;
  unsigned long currentMillis;
  shutter.stop();

  currentMillis = millis();
  if (currentMillis - startMillis >= 100) {
    startMillis = currentMillis;
  }
  shutter.setCurrentPosition(shutter.currentPosition());
#ifdef shutter_mot_auto_poff
  digitalWrite(shutter_en, false);
  shutter.disableOutputs();
#endif
}

void stepper::set_positon(long pos) {
  shutter.setCurrentPosition(pos);
}

bool stepper::isrun() {
  return shutter.isRunning();
}