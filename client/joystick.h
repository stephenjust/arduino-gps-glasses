#ifndef _JOY_H_
#define _JOY_H_

extern int16_t joy_center_x;
extern int16_t joy_center_y;

void initialize_joystick();
uint8_t process_joystick(int16_t *dx, int16_t *dy);

#endif
