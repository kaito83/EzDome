#ifndef steppers_h
#define steppers_h


class stepper {
private:

public:
  void init(int speed, int accel);
  void move(long move);

  void set_positon(long pos);
  long position();
  bool isrun();
  void forcestop();
  void softstop();
};
#endif