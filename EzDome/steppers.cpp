#include "FastAccelStepper.h";
#include "steppers.h";
#include "EzDome.h";

//Stepper layer is responsible for the motor controlling


FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *rotator = NULL;


//initing motor
void stepper::init(float speed, float accel) {
  engine.init();

  rotator = engine.stepperConnectToPin(rotator_stp);
  rotator->setDirectionPin(rotator_dir, rotator_dir_invert);
  rotator->setEnablePin(rotator_en, rotator_en_invert);
  rotator->setAutoEnable(true);
  rotator->setAcceleration(accel);
  rotator->setSpeedInHz(speed);
}

// rotor movement always find shortest route
long stepper::move(long move) {
  long current = rotator->getCurrentPosition();
  long delta = move - current;

  if (delta > rot_full_rotation / 2) {
    delta -= rot_full_rotation;
  } else if (delta < -rot_full_rotation / 2) {
    delta += rot_full_rotation;
  }
  long target = current + delta;
  rotator->moveTo(target);
  return delta;
}


//stoping the motor
void stepper::stop() {
  rotator->stopMove();
}

//disable motors enable pin
void stepper::disable(bool S) {
  digitalWrite(rotator_en, S);
}

//updating motor position
void stepper::set_position(long pos) {
  rotator->setCurrentPosition(pos);
}

//query motor position
long stepper::get_position() {
  long pos;
  pos = rotator->getCurrentPosition();
  if (pos >= rot_full_rotation)
    pos -= rot_full_rotation;
  if (pos < 0)
    pos += rot_full_rotation;
  return pos;
}

//check motor is runing
bool stepper::isrun() {
  return rotator->isRunning();
}
