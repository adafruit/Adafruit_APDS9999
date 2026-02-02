/*
 *  test_APDS9999.ino
 *
 *  Example sketch for the Adafruit APDS-9999 library.
 *  Checks if the sensor is connected and functioning.
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit APDS-9999 Test"));

  if (!apds.begin()) {
    Serial.println(F("Failed to find APDS-9999 chip"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("APDS-9999 Found!"));

  // Part ID test
  Serial.print(F("Part ID: 0x"));
  Serial.print(apds.getPartID(), HEX);
  Serial.println(apds.getPartID() == 0x0C ? F(" [PASS]") : F(" [FAIL - expected 0x0C]"));

  // Revision ID test
  Serial.print(F("Revision ID: 0x"));
  Serial.println(apds.getRevisionID(), HEX);

  // MAIN_CTRL register tests
  Serial.println(F("\n--- MAIN_CTRL Tests ---"));

  // Light sensor enable test
  apds.enableLightSensor(true);
  Serial.print(F("LS Enable (set true): "));
  Serial.println(apds.lightSensorEnabled() ? F("[PASS]") : F("[FAIL]"));

  apds.enableLightSensor(false);
  Serial.print(F("LS Enable (set false): "));
  Serial.println(!apds.lightSensorEnabled() ? F("[PASS]") : F("[FAIL]"));

  // Proximity sensor enable test
  apds.enableProximitySensor(true);
  Serial.print(F("PS Enable (set true): "));
  Serial.println(apds.proximitySensorEnabled() ? F("[PASS]") : F("[FAIL]"));

  apds.enableProximitySensor(false);
  Serial.print(F("PS Enable (set false): "));
  Serial.println(!apds.proximitySensorEnabled() ? F("[PASS]") : F("[FAIL]"));

  // RGB mode test
  apds.setRGBMode(true);
  Serial.print(F("RGB Mode (set true): "));
  Serial.println(apds.getRGBMode() ? F("[PASS]") : F("[FAIL]"));

  apds.setRGBMode(false);
  Serial.print(F("RGB Mode (set false): "));
  Serial.println(!apds.getRGBMode() ? F("[PASS]") : F("[FAIL]"));

  // LED Pulses test
  Serial.println(F("\n--- PS_PULSES Tests ---"));
  uint8_t testPulses[] = {1, 8, 64, 255};
  for (int i = 0; i < 4; i++) {
    apds.setLEDPulses(testPulses[i]);
    uint8_t readback = apds.getLEDPulses();
    Serial.print(F("LED Pulses (set "));
    Serial.print(testPulses[i]);
    Serial.print(F("): "));
    Serial.println(readback == testPulses[i] ? F("[PASS]") : F("[FAIL]"));
  }

  // LS_GAIN register tests
  Serial.println(F("\n--- LS_GAIN Tests ---"));

  // Test 1X gain
  apds.setLSGain(APDS9999_LS_GAIN_1X);
  Serial.print(F("LS Gain 1X: "));
  Serial.println(apds.getLSGain() == APDS9999_LS_GAIN_1X ? F("[PASS]") : F("[FAIL]"));

  // Test 6X gain
  apds.setLSGain(APDS9999_LS_GAIN_6X);
  Serial.print(F("LS Gain 6X: "));
  Serial.println(apds.getLSGain() == APDS9999_LS_GAIN_6X ? F("[PASS]") : F("[FAIL]"));

  // Test 18X gain
  apds.setLSGain(APDS9999_LS_GAIN_18X);
  Serial.print(F("LS Gain 18X: "));
  Serial.println(apds.getLSGain() == APDS9999_LS_GAIN_18X ? F("[PASS]") : F("[FAIL]"));

  // PS_VCSEL register tests - LED Current
  Serial.println(F("\n--- PS_VCSEL Tests ---"));

  apds.setLEDCurrent(APDS9999_LED_CURRENT_10MA);
  Serial.print(F("LED Current (set 10mA): "));
  Serial.println(apds.getLEDCurrent() == APDS9999_LED_CURRENT_10MA ? F("[PASS]") : F("[FAIL]"));

  apds.setLEDCurrent(APDS9999_LED_CURRENT_25MA);
  Serial.print(F("LED Current (set 25mA): "));
  Serial.println(apds.getLEDCurrent() == APDS9999_LED_CURRENT_25MA ? F("[PASS]") : F("[FAIL]"));

  // PS_VCSEL register tests - LED Frequency
  apds.setLEDFrequency(APDS9999_LED_FREQ_60KHZ);
  Serial.print(F("LED Freq (set 60kHz): "));
  Serial.println(apds.getLEDFrequency() == APDS9999_LED_FREQ_60KHZ ? F("[PASS]") : F("[FAIL]"));

  apds.setLEDFrequency(APDS9999_LED_FREQ_80KHZ);
  Serial.print(F("LED Freq (set 80kHz): "));
  Serial.println(apds.getLEDFrequency() == APDS9999_LED_FREQ_80KHZ ? F("[PASS]") : F("[FAIL]"));

  apds.setLEDFrequency(APDS9999_LED_FREQ_100KHZ);
  Serial.print(F("LED Freq (set 100kHz): "));
  Serial.println(apds.getLEDFrequency() == APDS9999_LED_FREQ_100KHZ ? F("[PASS]") : F("[FAIL]"));

  // LS_MEAS_RATE register tests - Resolution
  Serial.println(F("\n--- LS_MEAS_RATE Tests ---"));

  apds.setLSResolution(APDS9999_LS_RES_20BIT);
  Serial.print(F("LS Resolution (20-bit): "));
  Serial.println(apds.getLSResolution() == APDS9999_LS_RES_20BIT ? F("[PASS]") : F("[FAIL]"));

  apds.setLSResolution(APDS9999_LS_RES_16BIT);
  Serial.print(F("LS Resolution (16-bit): "));
  Serial.println(apds.getLSResolution() == APDS9999_LS_RES_16BIT ? F("[PASS]") : F("[FAIL]"));

  apds.setLSResolution(APDS9999_LS_RES_13BIT);
  Serial.print(F("LS Resolution (13-bit): "));
  Serial.println(apds.getLSResolution() == APDS9999_LS_RES_13BIT ? F("[PASS]") : F("[FAIL]"));

  // LS_MEAS_RATE register tests - Measurement Rate
  apds.setLSMeasRate(APDS9999_LS_RATE_100MS);
  Serial.print(F("LS Meas Rate (100ms): "));
  Serial.println(apds.getLSMeasRate() == APDS9999_LS_RATE_100MS ? F("[PASS]") : F("[FAIL]"));

  apds.setLSMeasRate(APDS9999_LS_RATE_500MS);
  Serial.print(F("LS Meas Rate (500ms): "));
  Serial.println(apds.getLSMeasRate() == APDS9999_LS_RATE_500MS ? F("[PASS]") : F("[FAIL]"));

  apds.setLSMeasRate(APDS9999_LS_RATE_2000MS);
  Serial.print(F("LS Meas Rate (2000ms): "));
  Serial.println(apds.getLSMeasRate() == APDS9999_LS_RATE_2000MS ? F("[PASS]") : F("[FAIL]"));

  // Reset to sensible defaults for loop
  apds.setLSResolution(APDS9999_LS_RES_18BIT);
  apds.setLSMeasRate(APDS9999_LS_RATE_100MS);

  // PS_MEAS_RATE register tests - PS Resolution
  Serial.println(F("\n--- PS_MEAS_RATE Tests ---"));

  apds.setPSResolution(APDS9999_PS_RES_8BIT);
  Serial.print(F("PS Resolution (set 8-bit): "));
  Serial.println(apds.getPSResolution() == APDS9999_PS_RES_8BIT ? F("[PASS]") : F("[FAIL]"));

  apds.setPSResolution(APDS9999_PS_RES_10BIT);
  Serial.print(F("PS Resolution (set 10-bit): "));
  Serial.println(apds.getPSResolution() == APDS9999_PS_RES_10BIT ? F("[PASS]") : F("[FAIL]"));

  apds.setPSResolution(APDS9999_PS_RES_11BIT);
  Serial.print(F("PS Resolution (set 11-bit): "));
  Serial.println(apds.getPSResolution() == APDS9999_PS_RES_11BIT ? F("[PASS]") : F("[FAIL]"));

  // PS_MEAS_RATE register tests - PS Measurement Rate
  apds.setPSMeasRate(APDS9999_PS_RATE_6MS);
  Serial.print(F("PS Meas Rate (set 6ms): "));
  Serial.println(apds.getPSMeasRate() == APDS9999_PS_RATE_6MS ? F("[PASS]") : F("[FAIL]"));

  apds.setPSMeasRate(APDS9999_PS_RATE_50MS);
  Serial.print(F("PS Meas Rate (set 50ms): "));
  Serial.println(apds.getPSMeasRate() == APDS9999_PS_RATE_50MS ? F("[PASS]") : F("[FAIL]"));

  apds.setPSMeasRate(APDS9999_PS_RATE_100MS);
  Serial.print(F("PS Meas Rate (set 100ms): "));
  Serial.println(apds.getPSMeasRate() == APDS9999_PS_RATE_100MS ? F("[PASS]") : F("[FAIL]"));

  Serial.println(F("\n--- Tests Complete ---"));

  // Enable proximity sensor for continuous reading
  apds.enableProximitySensor(true);
  
  // Enable light sensor with RGB mode for RGBIR readings
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  delay(500);  // Wait for measurement
}

void loop() {
  Serial.print(F("Proximity: "));
  Serial.print(apds.readProximity());
  Serial.print(F("  Overflow: "));
  Serial.println(apds.getProximityOverflow() ? F("YES") : F("no"));
  
  // Read and print RGB+IR data
  uint32_t r, g, b, ir;
  if (apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.print("R:"); Serial.print(r);
    Serial.print(" G:"); Serial.print(g);
    Serial.print(" B:"); Serial.print(b);
    Serial.print(" IR:"); Serial.println(ir);
  } else {
    Serial.println("RGBIR read failed!");
  }
  
  delay(100);
}
