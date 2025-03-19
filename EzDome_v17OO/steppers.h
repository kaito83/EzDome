#ifndef steppers_h
#define steppers_h

class stepper {
public:
  void init(float speed, float accel);
  long move(long move);
  void run();
  void disable(bool S);
  void set_positon(long pos);
  long position();
  bool isrun();
  void stop();
};
#endif