#include "FastAccelStepper.h";
#include "steppers.h";
#include "EzShutter.h";


FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *shutter = NULL;

void stepper::init(int speed, int accel) {
  engine.init();

  shutter = engine.stepperConnectToPin(shutter_stp);
  shutter->setDirectionPin(shutter_dir, shutter_dir_invert);
  shutter->setEnablePin(shutter_en, shutter_en_invert);
  shutter->setAutoEnable(true);
  shutter->setAcceleration(accel);
  shutter->setSpeedInHz(speed);
}

void stepper::move(long move) {
  shutter->moveTo(move);
}

void stepper::enable(bool en)
{
  shutter->setAutoEnable(en);
}

long stepper::position() {
  return shutter->getCurrentPosition();
}

void stepper::forcestop() {
  shutter->forceStop();
}

void stepper::softstop() {
  enable(false);
  shutter->stopMove();
}

void stepper::set_positon(long pos) {

  shutter->setPositionAfterCommandsCompleted(pos);
}

bool stepper::isrun() {
  return shutter->isRunning();
}