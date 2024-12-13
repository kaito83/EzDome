#include <AccelStepper.h>;
#include <Wire.h>;
#include "EzShutter.h";

AccelStepper shutter(AccelStepper::DRIVER, shutter_stp, shutter_dir);

void shut_init(float speed, float accel) {
  shutter.setEnablePin(shutter_en);
  shutter.setMaxSpeed(speed);
  shutter.setAcceleration(accel);
  shutter.setEnablePin(shutter_en);
  shutter.setPinsInverted(shutter_dir_invert, shutter_stp_invert, shutter_en_invert);
  shutter.enableOutputs();
}

void shut_move(long move) {
  shutter.moveTo(move);
}

void shut_run() {
  shutter.run();
}

long shut_position() {
  return shutter.currentPosition();
}

void shut_stop() {
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

void shut_set_positon(long pos) {
  shutter.setCurrentPosition(pos);
}

bool shut_isrun() {
  return shutter.isRunning();
}