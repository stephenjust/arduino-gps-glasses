#include <Arduino.h>

#include "ledon.h"

void map_to_glasses(int heading) {
    
    // Replace these with accessors to a global pin variables.
    int a0 = 22;
    int a1 = 23;
    int a2 = 24;
    //Used to enable certain leds
    int v[] = {0, 0, 0};
    
    //Put these in a global initialization file?
    pinMode(a0, OUTPUT);
    pinMode(a1, OUTPUT);
    pinMode(a2, OUTPUT);
    
    
    int i = (heading/45) % 8; //i is the led number to enable.
    
    //Make i a bitfield 
    v[0] = (i & 0x1);
    v[1] = (i & 0x2);
    v[2] = (i & 0x4);
    if (v[0]) 
        digitalWrite(a0, HIGH);
    else
        digitalWrite(a0, LOW);

    if (v[1])
        digitalWrite(a1, HIGH);
    else
        digitalWrite(a1, LOW);

    if (v[2])
        digitalWrite(a2, HIGH);
    else
        digitalWrite(a2, LOW);    
    
}

 
