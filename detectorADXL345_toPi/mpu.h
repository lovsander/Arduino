#ifndef _MYMPU_H_
#define _MYMPU_H_
#include "defines.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL345_REG_DEVID               (0x00)    // Device ID
#define ADXL345_REG_THRESH_TAP          (0x1D)    // Tap threshold
#define ADXL345_REG_OFSX                (0x1E)    // X-axis offset
#define ADXL345_REG_OFSY                (0x1F)    // Y-axis offset
#define ADXL345_REG_OFSZ                (0x20)    // Z-axis offset
#define ADXL345_REG_DUR                 (0x21)    // Tap duration
#define ADXL345_REG_LATENT              (0x22)    // Tap latency
#define ADXL345_REG_WINDOW              (0x23)    // Tap window
#define ADXL345_REG_THRESH_ACT          (0x24)    // Activity threshold
#define ADXL345_REG_THRESH_INACT        (0x25)    // Inactivity threshold
#define ADXL345_REG_TIME_INACT          (0x26)    // Inactivity time
#define ADXL345_REG_ACT_INACT_CTL       (0x27)    // Axis enable control for activity and inactivity detection
#define ADXL345_REG_THRESH_FF           (0x28)    // Free-fall threshold
#define ADXL345_REG_TIME_FF             (0x29)    // Free-fall time
#define ADXL345_REG_TAP_AXES            (0x2A)    // Axis control for single/double tap
#define ADXL345_REG_ACT_TAP_STATUS      (0x2B)    // Source for single/double tap
#define ADXL345_REG_BW_RATE             (0x2C)    // Data rate and power mode control
#define ADXL345_REG_POWER_CTL           (0x2D)    // Power-saving features control
#define ADXL345_REG_INT_ENABLE          (0x2E)    // Interrupt enable control
#define ADXL345_REG_INT_MAP             (0x2F)    // Interrupt mapping control
#define ADXL345_REG_INT_SOURCE          (0x30)    // Source of interrupts
#define ADXL345_REG_DATA_FORMAT         (0x31)    // Data format control
#define ADXL345_REG_DATAX0              (0x32)    // X-axis data 0
#define ADXL345_REG_DATAX1              (0x33)    // X-axis data 1
#define ADXL345_REG_DATAY0              (0x34)    // Y-axis data 0
#define ADXL345_REG_DATAY1              (0x35)    // Y-axis data 1
#define ADXL345_REG_DATAZ0              (0x36)    // Z-axis data 0
#define ADXL345_REG_DATAZ1              (0x37)    // Z-axis data 1
#define ADXL345_REG_FIFO_CTL            (0x38)    // FIFO control
#define ADXL345_REG_FIFO_STATUS         (0x39)    // FIFO status
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL345_MG2G_MULTIPLIER (0.004)  // 4mg per lsb
/*=========================================================================*/

/* Used with register 0x2C (ADXL345_REG_BW_RATE) to set bandwidth */
typedef enum
{
  ADXL345_DATARATE_3200_HZ    = 0b1111, // 1600Hz Bandwidth   140µA IDD
  ADXL345_DATARATE_1600_HZ    = 0b1110, //  800Hz Bandwidth    90µA IDD
  ADXL345_DATARATE_800_HZ     = 0b1101, //  400Hz Bandwidth   140µA IDD
  ADXL345_DATARATE_400_HZ     = 0b1100, //  200Hz Bandwidth   140µA IDD
  ADXL345_DATARATE_200_HZ     = 0b1011, //  100Hz Bandwidth   140µA IDD
  ADXL345_DATARATE_100_HZ     = 0b1010, //   50Hz Bandwidth   140µA IDD
  ADXL345_DATARATE_50_HZ      = 0b1001, //   25Hz Bandwidth    90µA IDD
  ADXL345_DATARATE_25_HZ      = 0b1000, // 12.5Hz Bandwidth    60µA IDD
  ADXL345_DATARATE_12_5_HZ    = 0b0111, // 6.25Hz Bandwidth    50µA IDD
  ADXL345_DATARATE_6_25HZ     = 0b0110, // 3.13Hz Bandwidth    45µA IDD
  ADXL345_DATARATE_3_13_HZ    = 0b0101, // 1.56Hz Bandwidth    40µA IDD
  ADXL345_DATARATE_1_56_HZ    = 0b0100, // 0.78Hz Bandwidth    34µA IDD
  ADXL345_DATARATE_0_78_HZ    = 0b0011, // 0.39Hz Bandwidth    23µA IDD
  ADXL345_DATARATE_0_39_HZ    = 0b0010, // 0.20Hz Bandwidth    23µA IDD
  ADXL345_DATARATE_0_20_HZ    = 0b0001, // 0.10Hz Bandwidth    23µA IDD
  ADXL345_DATARATE_0_10_HZ    = 0b0000  // 0.05Hz Bandwidth    23µA IDD (default value)
} dataRate_t;

/* Used with register 0x31 (ADXL345_REG_DATA_FORMAT) to set g range */
typedef enum
{
  ADXL345_RANGE_16_G          = 0b11,   // +/- 16g
  ADXL345_RANGE_8_G           = 0b10,   // +/- 8g
  ADXL345_RANGE_4_G           = 0b01,   // +/- 4g
  ADXL345_RANGE_2_G           = 0b00    // +/- 2g (default value)
} range_t;

class Adafruit_ADXL345
{
  public:
    Adafruit_ADXL345(uint8_t clock, uint8_t miso, uint8_t mosi, uint8_t cs, int32_t sensorID = -1);

    bool       begin();
    void       setRange(range_t range);
    range_t    getRange(void);
    void       setDataRate(dataRate_t dataRate);
    dataRate_t getDataRate(void);
    uint8_t    getDeviceID(void);
    void       writeRegister(uint8_t reg, uint8_t value);
    uint8_t    readRegister(uint8_t reg);
    int16_t    read16(uint8_t reg);
    int16_t    getX(void), getY(void), getZ(void);
    uint32_t   calcAcc();
    void       UpdateRawAccel();
    void       UpdateRawAccel1();
    int16_t    GetRawAccX();
    int16_t    GetRawAccY();
    int16_t    GetRawAccZ();

  private:
    int32_t _sensorID;
    range_t _range;
    uint8_t _clk, _do, _di, _cs;
    int16_t x, y, z;
};

/**************************************************************************/
/*!
    @brief  Abstract away SPI receiver & transmitter
*/
/**************************************************************************/
static uint8_t spixfer(uint8_t clock, uint8_t miso, uint8_t mosi, uint8_t data) {
  uint8_t reply = 0;
  for (int i = 7; i >= 0; i--) {
    reply <<= 1;
    digitalWrite(clock, LOW);
    digitalWrite(mosi, data & (1 << i));
    digitalWrite(clock, HIGH);
    if (digitalRead(miso))
      reply |= 1;
  }
  return reply;
}
/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void Adafruit_ADXL345::writeRegister(uint8_t reg, uint8_t value) {
  digitalWrite(_cs, LOW);
  spixfer(_clk, _di, _do, reg);
  spixfer(_clk, _di, _do, value);
  digitalWrite(_cs, HIGH);
}
/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/
uint8_t Adafruit_ADXL345::readRegister(uint8_t reg) {
  reg |= 0x80; // read byte
  digitalWrite(_cs, LOW);
  spixfer(_clk, _di, _do, reg);
  uint8_t reply = spixfer(_clk, _di, _do, 0xFF);
  digitalWrite(_cs, HIGH);
  return reply;
}
/**************************************************************************/
/*!
    @brief  Reads 16-bits from the specified register
*/
/**************************************************************************/
int16_t Adafruit_ADXL345::read16(uint8_t reg) {
  reg |= 0x80 | 0x40; // read byte | multibyte
  digitalWrite(_cs, LOW);
  spixfer(_clk, _di, _do, reg);
  int16_t reply = spixfer(_clk, _di, _do, 0xFF)  | (spixfer(_clk, _di, _do, 0xFF) << 8);
  digitalWrite(_cs, HIGH);
  return reply;
}

void Adafruit_ADXL345::UpdateRawAccel()
{
  x = getX();
  y = getY();
  z = getZ();
}

void Adafruit_ADXL345::UpdateRawAccel1() {
  uint8_t reg = ADXL345_REG_DATAX0;
  reg |= 0x80 | 0x40; // read byte | multibyte
  digitalWrite(_cs, LOW);
  spixfer(_clk, _di, _do, reg);
  x = spixfer(_clk, _di, _do, 0xFF)  | (spixfer(_clk, _di, _do, 0xFF) << 8);
  y = spixfer(_clk, _di, _do, 0xFF)  | (spixfer(_clk, _di, _do, 0xFF) << 8);
  z = spixfer(_clk, _di, _do, 0xFF)  | (spixfer(_clk, _di, _do, 0xFF) << 8);
  digitalWrite(_cs, HIGH);
}

uint32_t Adafruit_ADXL345::calcAcc() {
  int32_t t;
  uint32_t val;
  t = x;
  val = t * t;
  t = y;
  val += t * t;
  t = z;
  val += t * t;
  return (sqrt(val));  
}

int16_t    Adafruit_ADXL345::GetRawAccX() {
  return x;
}
int16_t    Adafruit_ADXL345::GetRawAccY() {
  return y;
}
int16_t    Adafruit_ADXL345::GetRawAccZ() {
  return z;
}

/**************************************************************************/
/*!
    @brief  Reads the device ID (can be used to check connection)
*/
/**************************************************************************/
uint8_t Adafruit_ADXL345::getDeviceID(void) {
  // Check device ID register
  return readRegister(ADXL345_REG_DEVID);
}
/**************************************************************************/
/*!
    @brief  Gets the most recent X axis value
*/
/**************************************************************************/
int16_t Adafruit_ADXL345::getX(void) {
  return read16(ADXL345_REG_DATAX0);
}
/**************************************************************************/
/*!
    @brief  Gets the most recent Y axis value
*/
/**************************************************************************/
int16_t Adafruit_ADXL345::getY(void) {
  return read16(ADXL345_REG_DATAY0);
}
/**************************************************************************/
/*!
    @brief  Gets the most recent Z axis value
*/
/**************************************************************************/
int16_t Adafruit_ADXL345::getZ(void) {
  return read16(ADXL345_REG_DATAZ0);
}
/**************************************************************************/
/*!
    @brief  Instantiates a new ADXL345 class in SPI mode
*/
/**************************************************************************/
Adafruit_ADXL345::Adafruit_ADXL345(uint8_t clock, uint8_t miso, uint8_t mosi, uint8_t cs, int32_t sensorID) {
  _sensorID = sensorID;
  _range = ADXL345_RANGE_2_G;
  _cs = cs;
  _clk = clock;
  _do = mosi;
  _di = miso;
}

/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
*/
/**************************************************************************/
bool Adafruit_ADXL345::begin() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  pinMode(_clk, OUTPUT);
  digitalWrite(_clk, HIGH);
  pinMode(_do, OUTPUT);
  pinMode(_di, INPUT);
  /* Check connection */
  uint8_t deviceid = getDeviceID();
  if (deviceid != 0xE5)
  {
    /* No ADXL345 detected ... return false */
    return false;
  }
  // Enable measurements
  writeRegister(ADXL345_REG_POWER_CTL, 0x08);
  return true;
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer
*/
/**************************************************************************/
void Adafruit_ADXL345::setRange(range_t range)
{
  /* Read the data format register to preserve bits */
  uint8_t format = readRegister(ADXL345_REG_DATA_FORMAT);

  /* Update the data rate */
  format &= ~0x0F;
  format |= range;

  /* Make sure that the FULL-RES bit is enabled for range scaling */
  format |= 0x08;

  /* Write the register back to the IC */
  writeRegister(ADXL345_REG_DATA_FORMAT, format);

  /* Keep track of the current range (to avoid readbacks) */
  _range = range;
}

/**************************************************************************/
/*!
    @brief  Gets the g range for the accelerometer
*/
/**************************************************************************/
range_t Adafruit_ADXL345::getRange(void)
{
  /* Read the data format register to preserve bits */
  return (range_t)(readRegister(ADXL345_REG_DATA_FORMAT) & 0x03);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the ADXL345 (controls power consumption)
*/
/**************************************************************************/
void Adafruit_ADXL345::setDataRate(dataRate_t dataRate)
{
  /* Note: The LOW_POWER bits are currently ignored and we always keep
     the device in 'normal' mode */
  writeRegister(ADXL345_REG_BW_RATE, dataRate);
}

/**************************************************************************/
/*!
    @brief  Gets the data rate for the ADXL345 (controls power consumption)
*/
/**************************************************************************/
dataRate_t Adafruit_ADXL345::getDataRate(void)
{
  return (dataRate_t)(readRegister(ADXL345_REG_BW_RATE) & 0x0F);
}


#endif // _MYMPU_H_
