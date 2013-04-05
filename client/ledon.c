#include <Arduino.h>

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
    
    
    int i; //i is the led number to enable.
    
    i = map(heading, 0, 360, 0, 7);
    
    //Make i a bitfield 
    if (i & 0x1) { v[0] = 1;} else {v[0] = 0;}
    if (i & 0x2) { v[1] = 1;} else {v[1] = 0;}
    if (i & 0x4) { v[2] = 1;} else {v[2] = 0;}
    if (v[0]) 
        digitalWrite(a0, HIGH);
    if (v[1])
        digitalWrite(a1, HIGH);
    if (v[2])
        digitalWrite(a2, HIGH);
    
    
}



void loop() {
    
    
}
 
