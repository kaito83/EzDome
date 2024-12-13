#ifndef steppers_h
#define steppers_h

void shut_init(float speed, float accel);
void shut_move(long move);
void shut_run();
void shut_disable(bool S);
void shut_set_positon(long pos);
long shut_position();
bool shut_isrun();
void shut_stop();

#endif