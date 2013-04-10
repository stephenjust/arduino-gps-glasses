#include <Arduino.h>

#include "joystick.h"

// Arduino analog input pin for the horizontal on the joystick.
const uint8_t joy_pin_x = 0;
// Arduino analog input pin for the vertical on the joystick.
const uint8_t joy_pin_y = 1;
// Digital pin for the joystick button on the Arduino.
const uint8_t joy_pin_button = 4;

// Center point of the joystick - analog reads from the Arduino.
int16_t joy_center_x = 512;
int16_t joy_center_y = 512;


// button state: 0 not pressed, 1 pressed
uint8_t prev_button_state = 0;

// time of last sampling of button state
uint32_t button_prev_time = 0;

// only after this much time has passed is the state sampled.
uint32_t button_sample_delay = 200;

void initialize_joystick() {
    // Initialize the button pin, turn on pullup resistor
    pinMode(joy_pin_button, INPUT);
    digitalWrite(joy_pin_button, HIGH);

    // Center Joystick
    joy_center_x = analogRead(joy_pin_x);
    joy_center_y = analogRead(joy_pin_y);
}


/*
  Read the joystick position, and return the x, y displacement from the zero
  position.  The joystick has to be at least 4 units away from zero before a
  non-zero displacement is returned.  This filters out the centering errors that
  occur when the joystick is released.

  Also, return 1 if the joystick button has been pushed, held for a minimum
  amount of time, and then released.  That is, a 1 is returned if a button
  select action has occurred.  

*/
uint8_t process_joystick(int16_t *dx, int16_t *dy) {
    int16_t joy_x;
    int16_t joy_y;
    uint8_t button_state;

    joy_x = -(analogRead(joy_pin_y) - joy_center_x);
    joy_y = -(analogRead(joy_pin_x) - joy_center_y);

    if (abs(joy_x) <= 4) {
        joy_x = 0;
    }

    if (abs(joy_y) <= 4) {
        joy_y = 0;
    }

    *dx = joy_x / 128;
    *dy = joy_y / 128;

    // first, don't even sample unless enough time has passed
    uint8_t have_sample = 0;
    uint32_t cur_time = millis();
    if ( cur_time < button_prev_time ) {
        // time inversion caused by wraparound, so reset
        button_prev_time = cur_time;
    }
    if ( button_prev_time == 0 || 
         button_prev_time + button_sample_delay < cur_time ) {
        // button pushed after suitable delay, so ok to return state
        button_prev_time = cur_time;
        have_sample = 1;
        button_state = LOW == digitalRead(joy_pin_button);
    }

    // if no sample, return no press result
    if ( ! have_sample ) { return 0; }

    // if prev and current state are same, no transition occurs
    if ( prev_button_state == button_state ) { return 0; }

    // are we waiting for push or release?
    if ( prev_button_state ) {
        // we got a release, return the press event
        prev_button_state = button_state;
        return 1;
    }

    // we got a press, so change state to waiting for release, but
    // don't signal the press event yet.
    prev_button_state = button_state;
    return 0;
}
