
#include <LSM303.h>
#include <Wire.h>
#include <math.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define MAG_ADDRESS            (0x3C >> 1)
#define ACC_ADDRESS_SA0_A_LOW  (0x30 >> 1)
#define ACC_ADDRESS_SA0_A_HIGH (0x32 >> 1)

int past_headings[3] = {0, 0, 0};
int past_heading_i = 0;

// Constructors ////////////////////////////////////////////////////////////////

LSM303::LSM303(void)
{
  // These are just some values for a particular unit; it is recommended that
  // a calibration be done for your particular unit.
    m_max.x = +1; m_max.y = +2; m_max.z = 0;
  m_min.x = -1; m_min.y = -2; m_min.z = -140;

  _device = LSM303_DEVICE_AUTO;
  acc_address = ACC_ADDRESS_SA0_A_LOW;

  io_timeout = 0;  // 0 = no timeout
  did_timeout = false;
}

// Public Methods //////////////////////////////////////////////////////////////

bool LSM303::timeoutOccurred()
{
  return did_timeout;
}

void LSM303::setTimeout(unsigned int timeout)
{
  io_timeout = timeout;
}

unsigned int LSM303::getTimeout()
{
  return io_timeout;
}

void LSM303::init(byte device, byte sa0_a)
{
  _device = device;
  switch (_device)
  {
    case LSM303DLH_DEVICE:
    case LSM303DLM_DEVICE:
      if (sa0_a == LSM303_SA0_A_LOW)
        acc_address = ACC_ADDRESS_SA0_A_LOW;
      else if (sa0_a == LSM303_SA0_A_HIGH)
        acc_address = ACC_ADDRESS_SA0_A_HIGH;
      else
        acc_address = (detectSA0_A() == LSM303_SA0_A_HIGH) ? ACC_ADDRESS_SA0_A_HIGH : ACC_ADDRESS_SA0_A_LOW;
      break;

    case LSM303DLHC_DEVICE:
      acc_address = ACC_ADDRESS_SA0_A_HIGH;
      break;

    default:
      // try to auto-detect device
      if (detectSA0_A() == LSM303_SA0_A_HIGH)
      {
        // if device responds on 0011001b (SA0_A is high), assume DLHC
        acc_address = ACC_ADDRESS_SA0_A_HIGH;
        _device = LSM303DLHC_DEVICE;
      }
      else
      {
        // otherwise, assume DLH or DLM (pulled low by default on Pololu boards); query magnetometer WHO_AM_I to differentiate these two
        acc_address = ACC_ADDRESS_SA0_A_LOW;
        _device = (readMagReg(LSM303_WHO_AM_I_M) == 0x3C) ? LSM303DLM_DEVICE : LSM303DLH_DEVICE;
      }
  }
}

// Turns on the LSM303's accelerometer and magnetometers and places them in normal
// mode.
void LSM303::enableDefault(void)
{
  // Enable Accelerometer
  // 0x27 = 0b00100111
  // Normal power mode, all axes enabled
  writeAccReg(LSM303_CTRL_REG1_A, 0x27);

  if (_device == LSM303DLHC_DEVICE)
    writeAccReg(LSM303_CTRL_REG4_A, 0x08); // DLHC: enable high resolution mode

  // Enable Magnetometer
  // 0x00 = 0b00000000
  // Continuous conversion mode
  writeMagReg(LSM303_MR_REG_M, 0x00);
  writeMagReg(LSM303_CRA_REG_M, 0x14);  // 0x14 = mag 30Hz output rate
}

// Writes an accelerometer register
void LSM303::writeAccReg(byte reg, byte value)
{
  Wire.beginTransmission(acc_address);
  Wire.write(reg);
  Wire.write(value);
  last_status = Wire.endTransmission();
}

// Reads an accelerometer register
byte LSM303::readAccReg(byte reg)
{
  byte value;

  Wire.beginTransmission(acc_address);
  Wire.write(reg);
  last_status = Wire.endTransmission();
  Wire.requestFrom(acc_address, (byte)1);
  value = Wire.read();
  Wire.endTransmission();

  return value;
}

// Writes a magnetometer register
void LSM303::writeMagReg(byte reg, byte value)
{
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  last_status = Wire.endTransmission();
}

// Reads a magnetometer register
byte LSM303::readMagReg(int reg)
{
  byte value;

  // if dummy register address (magnetometer Y/Z), use device type to determine actual address
  if (reg < 0)
  {
    switch (reg)
    {
      case LSM303_OUT_Y_H_M:
        reg = (_device == LSM303DLH_DEVICE) ? LSM303DLH_OUT_Y_H_M : LSM303DLM_OUT_Y_H_M;
        break;
      case LSM303_OUT_Y_L_M:
        reg = (_device == LSM303DLH_DEVICE) ? LSM303DLH_OUT_Y_L_M : LSM303DLM_OUT_Y_L_M;
        break;
      case LSM303_OUT_Z_H_M:
        reg = (_device == LSM303DLH_DEVICE) ? LSM303DLH_OUT_Z_H_M : LSM303DLM_OUT_Z_H_M;
        break;
      case LSM303_OUT_Z_L_M:
        reg = (_device == LSM303DLH_DEVICE) ? LSM303DLH_OUT_Z_L_M : LSM303DLM_OUT_Z_L_M;
        break;
    }
  }

  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(reg);
  last_status = Wire.endTransmission();
  Wire.requestFrom(MAG_ADDRESS, 1);
  value = Wire.read();
  Wire.endTransmission();

  return value;
}

void LSM303::setMagGain(magGain value)
{
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(LSM303_CRB_REG_M);
  Wire.write((byte) value);
  Wire.endTransmission();
}

// Reads the 3 accelerometer channels and stores them in vector a
void LSM303::readAcc(void)
{
  Wire.beginTransmission(acc_address);
  // assert the MSB of the address to get the accelerometer
  // to do slave-transmit subaddress updating.
  Wire.write(LSM303_OUT_X_L_A | (1 << 7));
  last_status = Wire.endTransmission();
  Wire.requestFrom(acc_address, (byte)6);

  unsigned int millis_start = millis();
  did_timeout = false;
  while (Wire.available() < 6) {
    if (io_timeout > 0 && ((unsigned int)millis() - millis_start) > io_timeout) {
      did_timeout = true;      return;
    }
  }
  
  /* Old Wire.Reads(). Going to swap all y and z here */
  
  byte xla = Wire.read();
  byte xha = Wire.read();
  byte yla = Wire.read();
  byte yha = Wire.read();
  byte zla = Wire.read();
  byte zha = Wire.read();
   
  
  //New wire reads. Don't really work 
  /*
  byte xla = Wire.read();
  byte xha = Wire.read();
  byte zla = Wire.read();
  byte zha = Wire.read();
  byte yla = Wire.read();
  byte yha = Wire.read();
  */
  
  // combine high and low bytes, then shift right to discard lowest 4 bits (which are meaningless)
  // GCC performs an arithmetic right shift for signed negative numbers, but this code will not work
  // if you port it to a compiler that does a logical right shift instead.
  a.x = ((int16_t)(xha << 8 | xla)) >> 4;
  a.y = ((int16_t)(yha << 8 | yla)) >> 4;
  a.z = ((int16_t)(zha << 8 | zla)) >> 4;
}

// Reads the 3 magnetometer channels and stores them in vector m
void LSM303::readMag(void)
{
  Wire.beginTransmission(MAG_ADDRESS);
  Wire.write(LSM303_OUT_X_H_M);
  last_status = Wire.endTransmission();
  Wire.requestFrom(MAG_ADDRESS, 6);

  unsigned int millis_start = millis();
  did_timeout = false;
  while (Wire.available() < 6) {
    if (io_timeout > 0 && ((unsigned int)millis() - millis_start) > io_timeout) {
      did_timeout = true;
      return;
    }
  }

  uint16_t xhm = Wire.read();
  uint16_t xlm = Wire.read();

  uint16_t yhm, ylm, zhm, zlm;

  if (_device == LSM303DLH_DEVICE)
  {
    // DLH: register address for Y comes before Z
    yhm = Wire.read();
    ylm = Wire.read();
    zhm = Wire.read();
    zlm = Wire.read();
  }
  else
  {
    // DLM, DLHC: register address for Z comes before Y
    zhm = Wire.read();
    zlm = Wire.read();
    yhm = Wire.read();
    ylm = Wire.read();

  }


  // combine high and low bytes
  m.x = (int16_t)(xhm << 8 | xlm);
  m.y = (int16_t)(yhm << 8 | ylm);
  m.z = (int16_t)(zhm << 8 | zlm);

#ifdef DEBUG_COMPASS
  Serial.println(m.x);
  Serial.println(m.y);
  Serial.println(m.z);
  Serial.println();
  delay(1000);
#endif
}

// Reads all 6 channels of the LSM303 and stores them in the object variables
void LSM303::read(void)
{
  readAcc();
  readMag();
}

// Returns a heading in degrees, compensated for pitch and roll
int LSM303::heading(void)
{
    //Need to normalize the vector so that trig isn't broken
    vector b = a;
    vector_normalize(&b);
    
    float pitch = asin(-b.x);
    float roll = asin(b.y/cos(pitch));

    
    float xh = (m.x+75+30) * cos(pitch) + m.z * sin(pitch);
    float yh = (m.x+105) * sin(roll) * sin(pitch)
        + (m.y-115) * cos(roll) - m.z * sin(roll) * cos(pitch);
    float zh = -(m.x+105) * cos(roll) * sin(pitch) + (m.y-115) * sin(roll)
        + m.z * cos(roll) * cos(pitch);

    int heading = (int)(180 * atan2(yh, xh)/PI) % 360;
    
    // Populate some fields for average heading calculation
    if (!past_headings[0]) past_headings[0] = heading;
    if (!past_headings[1]) past_headings[1] = heading;
    if (!past_headings[2]) past_headings[2] = heading;
    past_headings[past_heading_i] = heading;
    past_heading_i = (past_heading_i + 1) % 3;

    /* Debugging output. */
#ifdef DEBUG_COMPASS
    Serial.print("b.x");
    Serial.print(b.x);
    Serial.print("b.y");
    Serial.print(b.y);
    Serial.println();
    Serial.print("Pitch: ");
    Serial.print(pitch);
    Serial.print("Roll:");
    Serial.print(roll);
    Serial.print("Xh: ");
    Serial.print(xh);
    Serial.print("Yh: ");
    Serial.print(yh);
    Serial.print("Zh: ");
    Serial.print(zh);
    
    Serial.print("Heading: ");
    Serial.println(heading);
#endif

    // Return average of most three recent headings
    return ((int)(past_headings[0] + past_headings[1] + past_headings[2])/3) % 360;
}

void LSM303::vector_cross(const vector *a,const vector *b, vector *out)
{
  out->x = a->y*b->z - a->z*b->y;
  out->y = a->z*b->x - a->x*b->z;
  out->z = a->x*b->y - a->y*b->x;
}

float LSM303::vector_dot(const vector *a,const vector *b)
{
  return a->x*b->x+a->y*b->y+a->z*b->z;
}

void LSM303::vector_normalize(vector *a)
{
  float mag = sqrt(vector_dot(a,a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}

// Private Methods //////////////////////////////////////////////////////////////

byte LSM303::detectSA0_A(void)
{
  Wire.beginTransmission(ACC_ADDRESS_SA0_A_LOW);
  Wire.write(LSM303_CTRL_REG1_A);
  last_status = Wire.endTransmission();
  Wire.requestFrom(ACC_ADDRESS_SA0_A_LOW, 1);
  if (Wire.available())
  {
    Wire.read();
    return LSM303_SA0_A_LOW;
  }
  else
    return LSM303_SA0_A_HIGH;
}
