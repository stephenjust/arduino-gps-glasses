#include <Arduino.h>

void setup() {
    int a0 = 22;
    int a1 = 23;
    int a2 = 24;
    //Used to enable certain leds
    int v[] = {0, 0, 0};
    
    pinMode(a0, OUTPUT);
    pinMode(a1, OUTPUT);
    pinMode(a2, OUTPUT);
    while (1) {
    for (int i = 0; i <8; i++){
    
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
    
    delay(2000);
    
    digitalWrite(a0, LOW);
    digitalWrite(a1, LOW);
    digitalWrite(a2, LOW);
    
    }
    }
}



void loop() {
    
    
}
