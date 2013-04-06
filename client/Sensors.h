#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <Arduino.h>
#include <Wire.h>

class Sensors {
public:
	Sensors();
	static void writeTo(int destination, byte address, byte val); // Provides the functionality to write to a I2C device given its address (destiniation), the destination register (address), and the value we are writing (val)
	static void readFrom(int destination, byte address, int num, byte _buff[]); // Provides the functionality to read from a I2C device given its address (destination), the buffer we are loading said information into (_buff) from a specified register (address), a number of bytes (num)
	static int getTime(); // Returns the time since sensor ini in seconds
	static unsigned long int getLongTime(); // Returns the time since sensor ini in miliseconds
	static volatile bool sensorSecond; // Provides a method for reading/writing values every second
private:
	static unsigned long int time; // Stores the time as used in above functions
};

#endif
