#ifndef steppers_h
#define steppers_h


class stepper {
private:

public:
  void init(float speed, float accel);
  void move(long move);
  void run();
  void disable(bool S);
  void set_positon(long pos);
  long position();
  bool isrun();
  void stop();
};
#endif