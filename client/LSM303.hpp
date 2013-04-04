#ifndef LSM303_H
#define LSM303_H

/* Definitions for LSM303 tilt-compensated compass module */

#define LSM303_SCALE 2  // accel full-scale, should be 2, 4, or 8

/* LSM303 Address definitions */
#define LSM303_MAG  0x1E  // assuming SA0 grounded
#define LSM303_ACC  0x18  // assuming SA0 grounded

#define LSM303_X 0
#define LSM303_Y 1
#define LSM303_Z 2

/* LSM303 Register definitions */
#define LSM303_CTRL_REG1_A 0x20
#define LSM303_CTRL_REG2_A 0x21
#define LSM303_CTRL_REG3_A 0x22
#define LSM303_CTRL_REG4_A 0x23
#define LSM303_CTRL_REG5_A 0x24
#define LSM303_HP_FILTER_RESET_A 0x25
#define LSM303_REFERENCE_A 0x26
#define LSM303_STATUS_REG_A 0x27
#define LSM303_OUT_X_L_A 0x28
#define LSM303_OUT_X_H_A 0x29
#define LSM303_OUT_Y_L_A 0x2A
#define LSM303_OUT_Y_H_A 0x2B
#define LSM303_OUT_Z_L_A 0x2C
#define LSM303_OUT_Z_H_A 0x2D
#define LSM303_INT1_CFG_A 0x30
#define LSM303_INT1_SOURCE_A 0x31
#define LSM303_INT1_THS_A 0x32
#define LSM303_INT1_DURATION_A 0x33
#define LSM303_CRA_REG_M 0x00
#define LSM303_CRB_REG_M 0x01
#define LSM303_MR_REG_M 0x02
#define LSM303_OUT_X_H_M 0x03
#define LSM303_OUT_X_L_M 0x04
#define LSM303_OUT_Y_H_M 0x05
#define LSM303_OUT_Y_L_M 0x06
#define LSM303_OUT_Z_H_M 0x07
#define LSM303_OUT_Z_L_M 0x08
#define LSM303_SR_REG_M 0x09
#define LSM303_IRA_REG_M 0x0A
#define LSM303_IRB_REG_M 0x0B
#define LSM303_IRC_REG_M 0x0C

class LSM303 {

  /* Global variables */
  int accel[3];  // we'll store the raw acceleration values here
  int mag[3];  // raw magnetometer values stored here
  float realAccel[3];  // calculated acceleration values here

public:
  void init();

  void write(byte data, byte address);

  byte read(byte address);

  int * getAccel();

  float * getRealAccel();

  int * getMag();

  int getHeading();

  float getTiltHeading();

  void printValues();

};

#endif
