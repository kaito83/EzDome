#ifndef steppers_h
#define steppers_h

class stepper {
public:
  void init(float speed, float accel);
  long move(long move);
  void disable(bool S);
  void set_position(long pos);
  long get_position();
  bool isrun();
  void stop();
};
#endif