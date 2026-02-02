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
    @brief  Set the light sensor gain
    @param  gain The gain setting (1x, 3x, 6x, 9x, or 18x)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLSGain(apds9999_ls_gain_t gain) {
  Adafruit_BusIO_Register ls_gain_reg(i2c_dev, APDS9999_REG_LS_GAIN);
  Adafruit_BusIO_RegisterBits ls_gain_bits(&ls_gain_reg, 3, 0);
  return ls_gain_bits.write(gain);
}

/**************************************************************************/
/*!
    @brief  Get the current light sensor gain setting
    @return The current gain setting
*/
/**************************************************************************/
apds9999_ls_gain_t Adafruit_APDS9999::getLSGain() {
  Adafruit_BusIO_Register ls_gain_reg(i2c_dev, APDS9999_REG_LS_GAIN);
  Adafruit_BusIO_RegisterBits ls_gain_bits(&ls_gain_reg, 3, 0);
  return (apds9999_ls_gain_t)ls_gain_bits.read();
}

/**************************************************************************/
/*!
    @brief  Set the light sensor ADC resolution
    @param  res The resolution setting (20, 19, 18, 17, 16, or 13 bit)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLSResolution(apds9999_ls_resolution_t res) {
  Adafruit_BusIO_Register ls_meas_rate(i2c_dev, APDS9999_REG_LS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ls_res(&ls_meas_rate, 3, 4);
  return ls_res.write(res);
}

/**************************************************************************/
/*!
    @brief  Get the current light sensor ADC resolution setting
    @return The current resolution setting
*/
/**************************************************************************/
apds9999_ls_resolution_t Adafruit_APDS9999::getLSResolution() {
  Adafruit_BusIO_Register ls_meas_rate(i2c_dev, APDS9999_REG_LS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ls_res(&ls_meas_rate, 3, 4);
  return (apds9999_ls_resolution_t)ls_res.read();
}

/**************************************************************************/
/*!
    @brief  Set the light sensor measurement rate
    @param  rate The measurement rate setting (25ms to 2000ms)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLSMeasRate(apds9999_ls_meas_rate_t rate) {
  Adafruit_BusIO_Register ls_meas_rate(i2c_dev, APDS9999_REG_LS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ls_rate(&ls_meas_rate, 3, 0);
  return ls_rate.write(rate);
}

/**************************************************************************/
/*!
    @brief  Get the current light sensor measurement rate setting
    @return The current measurement rate setting
*/
/**************************************************************************/
apds9999_ls_meas_rate_t Adafruit_APDS9999::getLSMeasRate() {
  Adafruit_BusIO_Register ls_meas_rate(i2c_dev, APDS9999_REG_LS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ls_rate(&ls_meas_rate, 3, 0);
  return (apds9999_ls_meas_rate_t)ls_rate.read();
}

/**************************************************************************/
/*!
    @brief  Set the proximity sensor ADC resolution
    @param  res The resolution setting (8, 9, 10, or 11 bit)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setPSResolution(apds9999_ps_resolution_t res) {
  Adafruit_BusIO_Register ps_meas_rate(i2c_dev, APDS9999_REG_PS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ps_res(&ps_meas_rate, 2, 3);
  return ps_res.write(res);
}

/**************************************************************************/
/*!
    @brief  Get the current proximity sensor ADC resolution setting
    @return The current resolution setting
*/
/**************************************************************************/
apds9999_ps_resolution_t Adafruit_APDS9999::getPSResolution() {
  Adafruit_BusIO_Register ps_meas_rate(i2c_dev, APDS9999_REG_PS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ps_res(&ps_meas_rate, 2, 3);
  return (apds9999_ps_resolution_t)ps_res.read();
}

/**************************************************************************/
/*!
    @brief  Set the proximity sensor measurement rate
    @param  rate The measurement rate setting (6ms to 400ms)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setPSMeasRate(apds9999_ps_meas_rate_t rate) {
  Adafruit_BusIO_Register ps_meas_rate(i2c_dev, APDS9999_REG_PS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ps_rate(&ps_meas_rate, 3, 0);
  return ps_rate.write(rate);
}

/**************************************************************************/
/*!
    @brief  Get the current proximity sensor measurement rate setting
    @return The current measurement rate setting
*/
/**************************************************************************/
apds9999_ps_meas_rate_t Adafruit_APDS9999::getPSMeasRate() {
  Adafruit_BusIO_Register ps_meas_rate(i2c_dev, APDS9999_REG_PS_MEAS_RATE);
  Adafruit_BusIO_RegisterBits ps_rate(&ps_meas_rate, 3, 0);
  return (apds9999_ps_meas_rate_t)ps_rate.read();
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

/**************************************************************************/
/*!
    @brief  Set the number of LED pulses for proximity sensing
    @param  pulses Number of pulses (0-255)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLEDPulses(uint8_t pulses) {
  Adafruit_BusIO_Register ps_pulses(i2c_dev, APDS9999_REG_PS_PULSES);
  return ps_pulses.write(pulses);
}

/**************************************************************************/
/*!
    @brief  Get the number of LED pulses for proximity sensing
    @return Number of pulses (0-255)
*/
/**************************************************************************/
uint8_t Adafruit_APDS9999::getLEDPulses() {
  Adafruit_BusIO_Register ps_pulses(i2c_dev, APDS9999_REG_PS_PULSES);
  return ps_pulses.read();
}

/**************************************************************************/
/*!
    @brief  Set the LED drive current for proximity sensing
    @param  current The LED current setting (apds9999_led_current_t)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLEDCurrent(apds9999_led_current_t current) {
  Adafruit_BusIO_Register ps_vcsel(i2c_dev, APDS9999_REG_PS_VCSEL);
  Adafruit_BusIO_RegisterBits led_i(&ps_vcsel, 3, 0);
  return led_i.write(current);
}

/**************************************************************************/
/*!
    @brief  Get the LED drive current for proximity sensing
    @return The LED current setting (apds9999_led_current_t)
*/
/**************************************************************************/
apds9999_led_current_t Adafruit_APDS9999::getLEDCurrent() {
  Adafruit_BusIO_Register ps_vcsel(i2c_dev, APDS9999_REG_PS_VCSEL);
  Adafruit_BusIO_RegisterBits led_i(&ps_vcsel, 3, 0);
  return (apds9999_led_current_t)led_i.read();
}

/**************************************************************************/
/*!
    @brief  Set the LED pulse frequency for proximity sensing
    @param  freq The LED frequency setting (apds9999_led_freq_t)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLEDFrequency(apds9999_led_freq_t freq) {
  Adafruit_BusIO_Register ps_vcsel(i2c_dev, APDS9999_REG_PS_VCSEL);
  Adafruit_BusIO_RegisterBits led_freq(&ps_vcsel, 3, 4);
  return led_freq.write(freq);
}

/**************************************************************************/
/*!
    @brief  Get the LED pulse frequency for proximity sensing
    @return The LED frequency setting (apds9999_led_freq_t)
*/
/**************************************************************************/
apds9999_led_freq_t Adafruit_APDS9999::getLEDFrequency() {
  Adafruit_BusIO_Register ps_vcsel(i2c_dev, APDS9999_REG_PS_VCSEL);
  Adafruit_BusIO_RegisterBits led_freq(&ps_vcsel, 3, 4);
  return (apds9999_led_freq_t)led_freq.read();
}

/**************************************************************************/
/*!
    @brief  Enable or disable proximity sensor interrupt
    @param  en True to enable, false to disable
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::enablePSInterrupt(bool en) {
  Adafruit_BusIO_Register int_cfg(i2c_dev, APDS9999_REG_INT_CFG);
  Adafruit_BusIO_RegisterBits ps_int_en(&int_cfg, 1, 0);
  return ps_int_en.write(en ? 1 : 0);
}

/**************************************************************************/
/*!
    @brief  Check if proximity sensor interrupt is enabled
    @return True if enabled
*/
/**************************************************************************/
bool Adafruit_APDS9999::psInterruptEnabled() {
  Adafruit_BusIO_Register int_cfg(i2c_dev, APDS9999_REG_INT_CFG);
  Adafruit_BusIO_RegisterBits ps_int_en(&int_cfg, 1, 0);
  return ps_int_en.read();
}

/**************************************************************************/
/*!
    @brief  Enable or disable light sensor interrupt
    @param  en True to enable, false to disable
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::enableLSInterrupt(bool en) {
  Adafruit_BusIO_Register int_cfg(i2c_dev, APDS9999_REG_INT_CFG);
  Adafruit_BusIO_RegisterBits ls_int_en(&int_cfg, 1, 2);
  return ls_int_en.write(en ? 1 : 0);
}

/**************************************************************************/
/*!
    @brief  Check if light sensor interrupt is enabled
    @return True if enabled
*/
/**************************************************************************/
bool Adafruit_APDS9999::lsInterruptEnabled() {
  Adafruit_BusIO_Register int_cfg(i2c_dev, APDS9999_REG_INT_CFG);
  Adafruit_BusIO_RegisterBits ls_int_en(&int_cfg, 1, 2);
  return ls_int_en.read();
}

/**************************************************************************/
/*!
    @brief  Set the light sensor interrupt channel
    @param  ch The channel to use for LS interrupt (IR, Green, Red, or Blue)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLSIntChannel(apds9999_ls_int_channel_t ch) {
  Adafruit_BusIO_Register int_cfg(i2c_dev, APDS9999_REG_INT_CFG);
  Adafruit_BusIO_RegisterBits ls_int_sel(&int_cfg, 2, 4);
  return ls_int_sel.write(ch);
}

/**************************************************************************/
/*!
    @brief  Get the current light sensor interrupt channel
    @return The current LS interrupt channel setting
*/
/**************************************************************************/
apds9999_ls_int_channel_t Adafruit_APDS9999::getLSIntChannel() {
  Adafruit_BusIO_Register int_cfg(i2c_dev, APDS9999_REG_INT_CFG);
  Adafruit_BusIO_RegisterBits ls_int_sel(&int_cfg, 2, 4);
  return (apds9999_ls_int_channel_t)ls_int_sel.read();
}

/**************************************************************************/
/*!
    @brief  Set the proximity sensor interrupt persistence
    @param  pers Persistence value (0-15, number of consecutive readings)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setPSPersistence(uint8_t pers) {
  Adafruit_BusIO_Register int_pst(i2c_dev, APDS9999_REG_INT_PST);
  Adafruit_BusIO_RegisterBits ps_pers(&int_pst, 4, 0);
  return ps_pers.write(pers & 0x0F);
}

/**************************************************************************/
/*!
    @brief  Get the proximity sensor interrupt persistence
    @return Persistence value (0-15)
*/
/**************************************************************************/
uint8_t Adafruit_APDS9999::getPSPersistence() {
  Adafruit_BusIO_Register int_pst(i2c_dev, APDS9999_REG_INT_PST);
  Adafruit_BusIO_RegisterBits ps_pers(&int_pst, 4, 0);
  return ps_pers.read();
}

/**************************************************************************/
/*!
    @brief  Set the light sensor interrupt persistence
    @param  pers Persistence value (0-15, number of consecutive readings)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setLSPersistence(uint8_t pers) {
  Adafruit_BusIO_Register int_pst(i2c_dev, APDS9999_REG_INT_PST);
  Adafruit_BusIO_RegisterBits ls_pers(&int_pst, 4, 4);
  return ls_pers.write(pers & 0x0F);
}

/**************************************************************************/
/*!
    @brief  Get the light sensor interrupt persistence
    @return Persistence value (0-15)
*/
/**************************************************************************/
uint8_t Adafruit_APDS9999::getLSPersistence() {
  Adafruit_BusIO_Register int_pst(i2c_dev, APDS9999_REG_INT_PST);
  Adafruit_BusIO_RegisterBits ls_pers(&int_pst, 4, 4);
  return ls_pers.read();
}

/**************************************************************************/
/*!
    @brief  Set the proximity sensor upper threshold for interrupt
    @param  threshold 11-bit threshold value (0-2047)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setPSThresholdHigh(uint16_t threshold) {
  Adafruit_BusIO_Register ps_thresh(i2c_dev, APDS9999_REG_PS_THRES_UP_0, 2, LSBFIRST);
  return ps_thresh.write(threshold & 0x07FF);
}

/**************************************************************************/
/*!
    @brief  Get the proximity sensor upper threshold
    @return 11-bit threshold value (0-2047)
*/
/**************************************************************************/
uint16_t Adafruit_APDS9999::getPSThresholdHigh() {
  Adafruit_BusIO_Register ps_thresh(i2c_dev, APDS9999_REG_PS_THRES_UP_0, 2, LSBFIRST);
  return ps_thresh.read() & 0x07FF;
}

/**************************************************************************/
/*!
    @brief  Set the proximity sensor lower threshold for interrupt
    @param  threshold 11-bit threshold value (0-2047)
    @return True if write succeeded
*/
/**************************************************************************/
bool Adafruit_APDS9999::setPSThresholdLow(uint16_t threshold) {
  Adafruit_BusIO_Register ps_thresh(i2c_dev, APDS9999_REG_PS_THRES_LOW_0, 2, LSBFIRST);
  return ps_thresh.write(threshold & 0x07FF);
}

/**************************************************************************/
/*!
    @brief  Get the proximity sensor lower threshold
    @return 11-bit threshold value (0-2047)
*/
/**************************************************************************/
uint16_t Adafruit_APDS9999::getPSThresholdLow() {
  Adafruit_BusIO_Register ps_thresh(i2c_dev, APDS9999_REG_PS_THRES_LOW_0, 2, LSBFIRST);
  return ps_thresh.read() & 0x07FF;
}

/**************************************************************************/
/*!
    @brief  Get proximity sensor interrupt status (reading clears flag)
    @return True if PS interrupt flag is set
*/
/**************************************************************************/
bool Adafruit_APDS9999::getPSInterruptStatus() {
  Adafruit_BusIO_Register main_status(i2c_dev, APDS9999_REG_MAIN_STATUS);
  Adafruit_BusIO_RegisterBits ps_int(&main_status, 1, 1);
  return ps_int.read();
}

/**************************************************************************/
/*!
    @brief  Get light sensor interrupt status (reading clears flag)
    @return True if LS interrupt flag is set
*/
/**************************************************************************/
bool Adafruit_APDS9999::getLSInterruptStatus() {
  Adafruit_BusIO_Register main_status(i2c_dev, APDS9999_REG_MAIN_STATUS);
  Adafruit_BusIO_RegisterBits ls_int(&main_status, 1, 4);
  return ls_int.read();
}
