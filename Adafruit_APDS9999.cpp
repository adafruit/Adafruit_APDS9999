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
