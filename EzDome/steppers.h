#ifndef steppers_h
#define steppers_h

void rot_init(float speed, float accel);
long rot_move(long move);
void rot_run();
void rot_disable(bool S);
void rot_set_positon(long pos);
long rot_position();
bool rot_isrun();
void rot_stop();

#endif