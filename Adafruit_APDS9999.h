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

/** Main status register bit masks (for getMainStatus()) */
#define APDS9999_STATUS_PS_DATA 0x01  ///< Proximity data ready
#define APDS9999_STATUS_PS_INT 0x02   ///< Proximity interrupt triggered
#define APDS9999_STATUS_PS_LOGIC 0x04 ///< Proximity logic signal state
#define APDS9999_STATUS_LS_DATA 0x08  ///< Light sensor data ready
#define APDS9999_STATUS_LS_INT 0x10   ///< Light sensor interrupt triggered
#define APDS9999_STATUS_POWER_ON 0x20 ///< Power-on reset occurred

/** LS Gain settings for register 0x05 */
typedef enum {
  APDS9999_LS_GAIN_1X = 0x00, ///< 1x gain
  APDS9999_LS_GAIN_3X = 0x01, ///< 3x gain
  APDS9999_LS_GAIN_6X = 0x02, ///< 6x gain
  APDS9999_LS_GAIN_9X = 0x03, ///< 9x gain
  APDS9999_LS_GAIN_18X = 0x04 ///< 18x gain
} apds9999_ls_gain_t;

/** LED drive current options for proximity sensor */
typedef enum {
  APDS9999_LED_CURRENT_10MA = 0x02,
  APDS9999_LED_CURRENT_25MA = 0x03
} apds9999_led_current_t;

/** LED pulse frequency options for proximity sensor */
typedef enum {
  APDS9999_LED_FREQ_60KHZ = 0x03,
  APDS9999_LED_FREQ_70KHZ = 0x04,
  APDS9999_LED_FREQ_80KHZ = 0x05,
  APDS9999_LED_FREQ_90KHZ = 0x06,
  APDS9999_LED_FREQ_100KHZ = 0x07
} apds9999_led_freq_t;

/** Light sensor resolution (ADC bit depth) for LS_MEAS_RATE register */
typedef enum {
  APDS9999_LS_RES_20BIT = 0x00, ///< 20-bit resolution (400ms conversion)
  APDS9999_LS_RES_19BIT = 0x01, ///< 19-bit resolution (200ms conversion)
  APDS9999_LS_RES_18BIT = 0x02, ///< 18-bit resolution (100ms conversion)
  APDS9999_LS_RES_17BIT = 0x03, ///< 17-bit resolution (50ms conversion)
  APDS9999_LS_RES_16BIT = 0x04, ///< 16-bit resolution (25ms conversion)
  APDS9999_LS_RES_13BIT = 0x05  ///< 13-bit resolution (3.125ms conversion)
} apds9999_ls_resolution_t;

/** Light sensor measurement rate for LS_MEAS_RATE register */
typedef enum {
  APDS9999_LS_RATE_25MS = 0x00,   ///< 25ms measurement rate
  APDS9999_LS_RATE_50MS = 0x01,   ///< 50ms measurement rate
  APDS9999_LS_RATE_100MS = 0x02,  ///< 100ms measurement rate (default)
  APDS9999_LS_RATE_200MS = 0x03,  ///< 200ms measurement rate
  APDS9999_LS_RATE_500MS = 0x04,  ///< 500ms measurement rate
  APDS9999_LS_RATE_1000MS = 0x05, ///< 1000ms measurement rate
  APDS9999_LS_RATE_2000MS = 0x06  ///< 2000ms measurement rate
} apds9999_ls_meas_rate_t;

/** Proximity sensor resolution (ADC bit depth) for PS_MEAS_RATE register */
typedef enum {
  APDS9999_PS_RES_8BIT = 0x00,  ///< 8-bit resolution
  APDS9999_PS_RES_9BIT = 0x01,  ///< 9-bit resolution
  APDS9999_PS_RES_10BIT = 0x02, ///< 10-bit resolution
  APDS9999_PS_RES_11BIT = 0x03  ///< 11-bit resolution (default)
} apds9999_ps_resolution_t;

/** Proximity sensor measurement rate for PS_MEAS_RATE register */
typedef enum {
  APDS9999_PS_RATE_6MS = 0x01,   ///< 6.25ms measurement rate
  APDS9999_PS_RATE_12MS = 0x02,  ///< 12.5ms measurement rate
  APDS9999_PS_RATE_25MS = 0x03,  ///< 25ms measurement rate
  APDS9999_PS_RATE_50MS = 0x04,  ///< 50ms measurement rate
  APDS9999_PS_RATE_100MS = 0x05, ///< 100ms measurement rate (default)
  APDS9999_PS_RATE_200MS = 0x06, ///< 200ms measurement rate
  APDS9999_PS_RATE_400MS = 0x07  ///< 400ms measurement rate
} apds9999_ps_meas_rate_t;

/** LS interrupt channel select for INT_CFG register */
typedef enum {
  APDS9999_INT_CH_IR = 0x00,    ///< IR channel for LS interrupt
  APDS9999_INT_CH_GREEN = 0x01, ///< Green channel for LS interrupt
  APDS9999_INT_CH_RED = 0x02,   ///< Red channel for LS interrupt
  APDS9999_INT_CH_BLUE = 0x03   ///< Blue channel for LS interrupt
} apds9999_ls_int_channel_t;

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

  uint8_t getPartID();
  uint8_t getRevisionID();

  // MAIN_CTRL register functions
  bool enableLightSensor(bool en);
  bool lightSensorEnabled();
  bool enableProximitySensor(bool en);
  bool proximitySensorEnabled();
  bool setRGBMode(bool en);
  bool getRGBMode();

  // LS_GAIN register functions
  bool setLSGain(apds9999_ls_gain_t gain);
  apds9999_ls_gain_t getLSGain();

  // LS_MEAS_RATE register functions
  bool setLSResolution(apds9999_ls_resolution_t res);
  apds9999_ls_resolution_t getLSResolution();
  bool setLSMeasRate(apds9999_ls_meas_rate_t rate);
  apds9999_ls_meas_rate_t getLSMeasRate();

  // PS_MEAS_RATE register functions
  bool setPSResolution(apds9999_ps_resolution_t res);
  apds9999_ps_resolution_t getPSResolution();
  bool setPSMeasRate(apds9999_ps_meas_rate_t rate);
  apds9999_ps_meas_rate_t getPSMeasRate();

  // Proximity data functions
  uint16_t readProximity();
  bool getProximityOverflow();

  // RGB+IR bulk data read
  bool getRGBIRData(uint32_t* r, uint32_t* g, uint32_t* b, uint32_t* ir);

  // Lux calculation
  float calculateLux(uint32_t green_count);

  // PS_PULSES register functions
  bool setLEDPulses(uint8_t pulses);
  uint8_t getLEDPulses();

  // PS_VCSEL register functions
  bool setLEDCurrent(apds9999_led_current_t current);
  apds9999_led_current_t getLEDCurrent();
  bool setLEDFrequency(apds9999_led_freq_t freq);
  apds9999_led_freq_t getLEDFrequency();

  // INT_CFG (0x19) register functions
  bool enablePSInterrupt(bool en);
  bool psInterruptEnabled();
  bool enableLSInterrupt(bool en);
  bool lsInterruptEnabled();
  bool setLSIntChannel(apds9999_ls_int_channel_t ch);
  apds9999_ls_int_channel_t getLSIntChannel();

  // INT_PST (0x1A) register functions
  bool setPSPersistence(uint8_t pers);
  uint8_t getPSPersistence();
  bool setLSPersistence(uint8_t pers);
  uint8_t getLSPersistence();

  // PS Thresholds (0x1B-0x1E) register functions
  bool setPSThresholdHigh(uint16_t threshold);
  uint16_t getPSThresholdHigh();
  bool setPSThresholdLow(uint16_t threshold);
  uint16_t getPSThresholdLow();

  // LS Thresholds (0x21-0x26) register functions
  bool setLSThresholdHigh(uint32_t threshold);
  uint32_t getLSThresholdHigh();
  bool setLSThresholdLow(uint32_t threshold);
  uint32_t getLSThresholdLow();

  // PS Cancellation (0x1F-0x20) register functions
  bool setPSCancellation(uint16_t value);
  uint16_t getPSCancellation();
  bool setPSAnalogCancellation(uint8_t value);
  uint8_t getPSAnalogCancellation();

  // Status reading (0x07)
  // Note: Reading status clears ALL status bits! Use getMainStatus() to read
  // once and check multiple flags with APDS9999_STATUS_* masks.
  uint8_t getMainStatus();
  bool getPSInterruptStatus();
  bool getLSInterruptStatus();
  bool isPSDataReady();
  bool isLSDataReady();

  // Sleep After Interrupt modes (MAIN_CTRL bits 6,5)
  bool setPSSleepAfterInterrupt(bool enable);
  bool getPSSleepAfterInterrupt();
  bool setLSSleepAfterInterrupt(bool enable);
  bool getLSSleepAfterInterrupt();

  // Software reset (MAIN_CTRL bit 4)
  void reset();

 private:
  Adafruit_I2CDevice* i2c_dev = NULL; ///< Pointer to I2C bus interface
};

#endif
