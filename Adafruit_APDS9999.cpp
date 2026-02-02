/*!
 *  @file Adafruit_APDS9999.cpp
 *
 *  @mainpage Adafruit APDS-9999 library
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Adafruit APDS-9999 library
 *
 * 	This is a library for the Adafruit APDS-9999 breakout:
 * 	http://www.adafruit.com/products/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor 'ladyada' Fried with assistance from Gemini
 *
 *  @section license License
 *
 *  MIT license, all text above must be included in any redistribution
 */

#include "Adafruit_APDS9999.h"

/*!
 *    @brief  Instantiates a new APDS-9999 class
 */
Adafruit_APDS9999::Adafruit_APDS9999() {}

/*!
 *    @brief  Destructor
 */
Adafruit_APDS9999::~Adafruit_APDS9999() {
  if (i2c_dev) {
    delete i2c_dev;
  }
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_addr
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_APDS9999::begin(uint8_t i2c_addr, TwoWire* wire) {
  if (i2c_dev) {
    delete i2c_dev;
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_addr, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // Check PART_ID register to verify we have the correct chip
  Adafruit_BusIO_Register chip_id(i2c_dev, APDS9999_REG_PART_ID);
  if (chip_id.read() != 0xC2) {
    return false;
  }

  return true;
}

/*!
 *    @brief  Gets the part ID (upper nibble of PART_ID register)
 *    @return Part ID value (should be 0x0C for APDS-9999)
 */
uint8_t Adafruit_APDS9999::getPartID() {
  Adafruit_BusIO_Register part_id_reg(i2c_dev, APDS9999_REG_PART_ID);
  Adafruit_BusIO_RegisterBits part_id_bits(&part_id_reg, 4, 4);
  return part_id_bits.read();
}

/*!
 *    @brief  Gets the revision ID (lower nibble of PART_ID register)
 *    @return Revision ID value
 */
uint8_t Adafruit_APDS9999::getRevisionID() {
  Adafruit_BusIO_Register part_id_reg(i2c_dev, APDS9999_REG_PART_ID);
  Adafruit_BusIO_RegisterBits rev_id_bits(&part_id_reg, 4, 0);
  return rev_id_bits.read();
}

/**************************************************************************/
/*!
    @brief  Enable or disable the light sensor (ALS/RGB)
    @param  en True to enable, false to disable
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::enableLightSensor(bool en) {
  Adafruit_BusIO_Register main_ctrl(i2c_dev, APDS9999_REG_MAIN_CTRL);
  Adafruit_BusIO_RegisterBits ls_en(&main_ctrl, 1, 1);
  return ls_en.write(en ? 1 : 0);
}

/**************************************************************************/
/*!
    @brief  Check if the light sensor is enabled
    @return True if enabled
*/
/**************************************************************************/
bool Adafruit_APDS9999::lightSensorEnabled() {
  Adafruit_BusIO_Register main_ctrl(i2c_dev, APDS9999_REG_MAIN_CTRL);
  Adafruit_BusIO_RegisterBits ls_en(&main_ctrl, 1, 1);
  return ls_en.read();
}

/**************************************************************************/
/*!
    @brief  Enable or disable the proximity sensor
    @param  en True to enable, false to disable
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::enableProximitySensor(bool en) {
  Adafruit_BusIO_Register main_ctrl(i2c_dev, APDS9999_REG_MAIN_CTRL);
  Adafruit_BusIO_RegisterBits ps_en(&main_ctrl, 1, 0);
  return ps_en.write(en ? 1 : 0);
}

/**************************************************************************/
/*!
    @brief  Check if the proximity sensor is enabled
    @return True if enabled
*/
/**************************************************************************/
bool Adafruit_APDS9999::proximitySensorEnabled() {
  Adafruit_BusIO_Register main_ctrl(i2c_dev, APDS9999_REG_MAIN_CTRL);
  Adafruit_BusIO_RegisterBits ps_en(&main_ctrl, 1, 0);
  return ps_en.read();
}

/**************************************************************************/
/*!
    @brief  Set RGB mode (vs ALS mode) for light sensor
    @param  en True for RGB mode, false for ALS mode
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setRGBMode(bool en) {
  Adafruit_BusIO_Register main_ctrl(i2c_dev, APDS9999_REG_MAIN_CTRL);
  Adafruit_BusIO_RegisterBits rgb_mode(&main_ctrl, 1, 2);
  return rgb_mode.write(en ? 1 : 0);
}

/**************************************************************************/
/*!
    @brief  Get current RGB mode setting
    @return True if RGB mode, false if ALS mode
*/
/**************************************************************************/
bool Adafruit_APDS9999::getRGBMode() {
  Adafruit_BusIO_Register main_ctrl(i2c_dev, APDS9999_REG_MAIN_CTRL);
  Adafruit_BusIO_RegisterBits rgb_mode(&main_ctrl, 1, 2);
  return rgb_mode.read();
}

/**************************************************************************/
/*!
    @brief  Read proximity sensor data (11-bit value)
    @return Proximity value (0-2047)
*/
/**************************************************************************/
uint16_t Adafruit_APDS9999::readProximity() {
  Adafruit_BusIO_Register ps_data(i2c_dev, APDS9999_REG_PS_DATA_0, 2, LSBFIRST);
  return ps_data.read() & 0x07FF;
}

/**************************************************************************/
/*!
    @brief  Check if proximity sensor has overflowed
    @return True if overflow occurred (bit 11 set)
*/
/**************************************************************************/
bool Adafruit_APDS9999::getProximityOverflow() {
  Adafruit_BusIO_Register ps_data(i2c_dev, APDS9999_REG_PS_DATA_0, 2, LSBFIRST);
  return (ps_data.read() & 0x0800) != 0;
}

/**************************************************************************/
/*!
    @brief  Read all RGB+IR data in a single 12-byte bulk read
    @param  r Pointer to store red channel (20-bit value)
    @param  g Pointer to store green channel (20-bit value)
    @param  b Pointer to store blue channel (20-bit value)
    @param  ir Pointer to store IR channel (20-bit value)
    @return True if read succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::getRGBIRData(uint32_t *r, uint32_t *g, uint32_t *b, uint32_t *ir) {
  uint8_t buffer[12];
  uint8_t reg = APDS9999_REG_LS_DATA_IR_0;  // 0x0A
  
  if (!i2c_dev->write_then_read(&reg, 1, buffer, 12)) {
    return false;
  }
  
  // Each channel: 3 bytes, LSB first, mask to 20 bits
  *ir = (buffer[0] | (buffer[1] << 8) | (buffer[2] << 16)) & 0x0FFFFF;
  *g  = (buffer[3] | (buffer[4] << 8) | (buffer[5] << 16)) & 0x0FFFFF;
  *b  = (buffer[6] | (buffer[7] << 8) | (buffer[8] << 16)) & 0x0FFFFF;
  *r  = (buffer[9] | (buffer[10] << 8) | (buffer[11] << 16)) & 0x0FFFFF;
  
  return true;
}
