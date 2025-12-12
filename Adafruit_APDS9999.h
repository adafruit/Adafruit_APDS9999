/*!
 *  @file Adafruit_APDS9999.h
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
 *  MIT license, all text above must be included in any redistribution
 */

#ifndef ADAFRUIT_APDS9999_H
#define ADAFRUIT_APDS9999_H

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Arduino.h>

#define APDS9999_I2CADDR_DEFAULT 0x52 ///< APDS-9999 default I2C address

#define APDS9999_REG_MAIN_CTRL 0x00       ///< Main control register
#define APDS9999_REG_PS_VCSEL 0x01        ///< PS VCSEL control register
#define APDS9999_REG_PS_PULSES 0x02       ///< PS pulses control register
#define APDS9999_REG_PS_MEAS_RATE 0x03    ///< PS measurement rate
#define APDS9999_REG_LS_MEAS_RATE 0x04    ///< LS measurement rate
#define APDS9999_REG_LS_GAIN 0x05         ///< LS gain control
#define APDS9999_REG_PART_ID 0x06         ///< Part ID register
#define APDS9999_REG_MAIN_STATUS 0x07     ///< Main status register
#define APDS9999_REG_PS_DATA_0 0x08       ///< PS data low byte
#define APDS9999_REG_PS_DATA_1 0x09       ///< PS data high byte
#define APDS9999_REG_LS_DATA_IR_0 0x0A    ///< IR data low byte
#define APDS9999_REG_LS_DATA_IR_1 0x0B    ///< IR data middle byte
#define APDS9999_REG_LS_DATA_IR_2 0x0C    ///< IR data high byte
#define APDS9999_REG_LS_DATA_GREEN_0 0x0D ///< Green data low byte
#define APDS9999_REG_LS_DATA_GREEN_1 0x0E ///< Green data middle byte
#define APDS9999_REG_LS_DATA_GREEN_2 0x0F ///< Green data high byte
#define APDS9999_REG_LS_DATA_BLUE_0 0x10  ///< Blue data low byte
#define APDS9999_REG_LS_DATA_BLUE_1 0x11  ///< Blue data middle byte
#define APDS9999_REG_LS_DATA_BLUE_2 0x12  ///< Blue data high byte
#define APDS9999_REG_LS_DATA_RED_0 0x13   ///< Red data low byte
#define APDS9999_REG_LS_DATA_RED_1 0x14   ///< Red data middle byte
#define APDS9999_REG_LS_DATA_RED_2 0x15   ///< Red data high byte
#define APDS9999_REG_INT_CFG 0x19         ///< Interrupt configuration
#define APDS9999_REG_INT_PST 0x1A         ///< Interrupt persistence
#define APDS9999_REG_PS_THRES_UP_0 0x1B   ///< PS upper threshold low byte
#define APDS9999_REG_PS_THRES_UP_1 0x1C   ///< PS upper threshold high byte
#define APDS9999_REG_PS_THRES_LOW_0 0x1D  ///< PS lower threshold low byte
#define APDS9999_REG_PS_THRES_LOW_1 0x1E  ///< PS lower threshold high byte
#define APDS9999_REG_PS_CAN_0 0x1F        ///< PS cancellation level low byte
#define APDS9999_REG_PS_CAN_1 0x20        ///< PS cancellation level high byte
#define APDS9999_REG_LS_THRES_UP_0 0x21   ///< LS upper threshold low byte
#define APDS9999_REG_LS_THRES_UP_1 0x22   ///< LS upper threshold middle byte
#define APDS9999_REG_LS_THRES_UP_2 0x23   ///< LS upper threshold high byte
#define APDS9999_REG_LS_THRES_LOW_0 0x24  ///< LS lower threshold low byte
#define APDS9999_REG_LS_THRES_LOW_1 0x25  ///< LS lower threshold middle byte
#define APDS9999_REG_LS_THRES_LOW_2 0x26  ///< LS lower threshold high byte
#define APDS9999_REG_LS_THRES_VAR 0x27    ///< LS variance threshold

/*!
 *  @brief  Class that stores state and functions for interacting with
 *          APDS-9999 Digital Proximity and RGB Sensor
 */
class Adafruit_APDS9999 {
 public:
  Adafruit_APDS9999();
  ~Adafruit_APDS9999();

  bool begin(uint8_t i2c_addr = APDS9999_I2CADDR_DEFAULT,
             TwoWire* wire = &Wire);

 private:
  Adafruit_I2CDevice* i2c_dev = NULL; ///< Pointer to I2C bus interface
};

#endif
