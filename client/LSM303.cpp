/* LSM303DLH Example Code
   by: Jim Lindblom
   SparkFun Electronics
   date: 9/6/11
   license: Creative commons share-alike v3.0
   
   Summary:
   Show how to calculate level and tilt-compensated heading using
   the snazzy LSM303DLH 3-axis magnetometer/3-axis accelerometer.
   
   Firmware:
   You can set the accelerometer's full-scale range by setting
   the SCALE constant to either 2, 4, or 8. This value is used
   in the initLSM303() function. For the most part, all other
   registers in the LSM303 will be at their default value.
   
   Use the LSM303_write() and LSM303_read() functions to write
   to and read from the LSM303's internal registers.
   
   Use getLSM303_accel() and getLSM303_mag() to get the acceleration
   and magneto values from the LSM303. You'll need to pass each of
   those functions an array, where the data will be stored upon
   return from the void.
   
   getHeading() calculates a heading assuming the sensor is level.
   A float between 0 and 360 is returned. You need to pass it a
   array with magneto values. 
   
   getTiltHeading() calculates a tilt-compensated heading.
   A float between 0 and 360 degrees is returned. You need
   to pass this function both a magneto and acceleration array.
   
   Headings are calculated as specified in AN3192:
   http://www.sparkfun.com/datasheets/Sensors/Magneto/Tilt%20Compensated%20Compass.pdf
   
   Hardware:
   I'm using SparkFun's LSM303 breakout. Only power and the two
   I2C lines are connected:
   LSM303 Breakout ---------- Arduino
         Vin                   5V
         GND                   GND
         SDA                   A4
         SCL                   A5
*/

#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#include "LSM303.hpp"

/**
 * Initialize the compass module
 *
 * Parameter is the full scale of the module, can be 2, 4, 8
 */
void LSM303::init(int fs) {
  write(0x27, LSM303_CTRL_REG1_A);/* 0x27 = normal power mode,
					  all accel axes on */
  if ((fs==8)||(fs==4))
    write((0x00 | (fs-fs/2-1)<<4), LSM303_CTRL_REG4_A);  // set full-scale
  else
    write(0x00, LSM303_CTRL_REG4_A);
  write(0x14, LSM303_CRA_REG_M);  // 0x14 = mag 30Hz output rate
  write(0x00, LSM303_MR_REG_M);  // 0x00 = continouous conversion mode
}

/**
 * Write to the compass module
 */
void LSM303::write(byte data, byte address) {
  if (address >= 0x20)
    Wire.beginTransmission(LSM303_ACC);
  else
    Wire.beginTransmission(LSM303_MAG);
    
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

byte LSM303::read(byte address) {
  byte temp;
  
  if (address >= 0x20)
    Wire.beginTransmission(LSM303_ACC);
  else
    Wire.beginTransmission(LSM303_MAG);
    
  Wire.write(address);
  
  if (address >= 0x20)
    Wire.requestFrom(LSM303_ACC, 1);
  else
    Wire.requestFrom(LSM303_MAG, 1);
  while(!Wire.available())
    ;
  temp = Wire.read();
  Wire.endTransmission();
  
  return temp;
}


void LSM303::getAccel(int * rawValues) {
  rawValues[LSM303_Z] = ((int)read(LSM303_OUT_X_L_A) << 8)
    | (read(LSM303_OUT_X_H_A));
  rawValues[LSM303_X] = ((int)read(LSM303_OUT_Y_L_A) << 8)
    | (read(LSM303_OUT_Y_H_A));
  rawValues[LSM303_Y] = ((int)read(LSM303_OUT_Z_L_A) << 8)
    | (read(LSM303_OUT_Z_H_A));  
  // had to swap those to right the data with the proper axis
}

void LSM303::printValues(int * magArray, int * accelArray) {
  /* print out mag and accel arrays all pretty-like */
  Serial.print(accelArray[LSM303_X], DEC);
  Serial.print("\t");
  Serial.print(accelArray[LSM303_Y], DEC);
  Serial.print("\t");
  Serial.print(accelArray[LSM303_Z], DEC);
  Serial.print("\t\t");
  
  Serial.print(magArray[LSM303_X], DEC);
  Serial.print("\t");
  Serial.print(magArray[LSM303_Y], DEC);
  Serial.print("\t");
  Serial.print(magArray[LSM303_Z], DEC);
  Serial.println();
}

float LSM303::getHeading(int * magValue) {
  // see section 1.2 in app note AN3192
  float heading = 180*atan2(magValue[LSM303_Y],
			    magValue[LSM303_X])/PI; /* assume pitch,
						       roll are 0 */
  
  if (heading <0)
    heading += 360;
  
  return heading;
}

float LSM303::getTiltHeading(int * magValue, float * accelValue) {
  // see appendix A in app note AN3192 
  float pitch = asin(-accelValue[LSM303_X]);
  float roll = asin(accelValue[LSM303_Y]/cos(pitch));
  
  float xh = magValue[LSM303_X] * cos(pitch) + magValue[LSM303_Z] * sin(pitch);
  float yh = magValue[LSM303_X] * sin(roll) * sin(pitch) + magValue[LSM303_Y] * cos(roll) - magValue[LSM303_Z] * sin(roll) * cos(pitch);
  float zh = -magValue[LSM303_X] * cos(roll) * sin(pitch) + magValue[LSM303_Y] * sin(roll) + magValue[LSM303_Z] * cos(roll) * cos(pitch);

  float heading = 180 * atan2(yh, xh)/PI;
  if (yh >= 0)
    return heading;
  else
    return (360 + heading);
}

void LSM303::getMag(int * rawValues) {
  Wire.beginTransmission(LSM303_MAG);
  Wire.write(LSM303_OUT_X_H_M);
  Wire.endTransmission();
  Wire.requestFrom(LSM303_MAG, 6);
  for (int i=0; i<3; i++)
    rawValues[i] = (Wire.read() << 8) | Wire.read();
}


