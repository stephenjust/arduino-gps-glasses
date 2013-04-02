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

// Static allocations
unsigned long int Sensors::time;

// Prepare the inturupt element
volatile bool Sensors::sensorSecond = 0;

Sensors::Sensors()
{
	Wire.begin(); // Start the wire interface
	time = millis(); // Record the start time
}

int Sensors::getTime()
{
	return (int)((float)(0.001 * (millis() - time))); // Return the time in seconds
}

unsigned long int Sensors::getLongTime()
{
	return millis() - time; // Return the time in miliseconds
}

void Sensors::writeTo(int destination, byte address, byte val) {
	Wire.beginTransmission(destination); // start transmission to device 
	Wire.write(address);             // send register address
	Wire.write(val);                 // send value to write
	Wire.endTransmission();         // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void Sensors::readFrom(int destination, byte address, int num, byte _buff[]) {
	Wire.beginTransmission(destination); // start transmission to device 
	Wire.write(address);             // sends address to read from
	Wire.endTransmission();         // end transmission
	
	delay(6);
	
	Wire.beginTransmission(destination); // start transmission to device
	Wire.requestFrom(destination, num);    // request 6 bytes from device
	
	int i = 0;
	while(Wire.available())         // device may send less than requested (abnormal)
	{ 
		_buff[i] = Wire.read();    // receive a byte
		i++;
	}
	
	Wire.endTransmission();         // end transmission
}

#endif
