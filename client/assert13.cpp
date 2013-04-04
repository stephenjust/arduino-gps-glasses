#include <Arduino.h>
#include "assert13.h"
// create a busy-loop delay, since timers are off
int hard_loop(unsigned long int n) {
  // this introduction of y and the Serial.print
  // are to mess with gcc's optimization of the loop
  int y = 0;
  while (n > 0) {
    y = y + n;
    n--;
  }
  Serial.print(""); 
  return y;
}

#define DELAY_COUNT 1000000L

/* assertion checker
   assert13(invariant, code);
if invariant is false (i.e. 0) then fail and enter
a hard loop with interrupts disabled and repeatedly
sending code to the serial monitor, while blinking 
the LED on pin 13.

There is a small window in which an interrupt could
occur, and in which a failure could call assert13.
How would we guard against this?
*/

void assert13(int invariant, int code) {
  unsigned long int count;
  if ( invariant ) { return; }
  Serial.println("Assertion failure");
  noInterrupts();
  pinMode(13, OUTPUT);
  while ( 1 ) {
    Serial.println(code);
    digitalWrite(13, LOW);
    // Serial.println("LOW");
    hard_loop( DELAY_COUNT );
    digitalWrite(13, HIGH);
    // Serial.println("HIGH");
    hard_loop( DELAY_COUNT );    
  }
}
