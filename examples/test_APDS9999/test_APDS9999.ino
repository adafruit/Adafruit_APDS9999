/*
 *  test_APDS9999.ino
 *
 *  Example sketch for the Adafruit APDS-9999 library.
 *  Checks if the sensor is connected and functioning.
 */

#include <Adafruit_APDS9999.h>

#define INT_PIN 2

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

  // Interrupt config tests
  Serial.println(F("\n--- INT_CFG Tests ---"));

  apds.enablePSInterrupt(true);
  Serial.print(F("PS Int Enable: "));
  Serial.println(apds.psInterruptEnabled() ? F("[PASS]") : F("[FAIL]"));
  apds.enablePSInterrupt(false);

  apds.enableLSInterrupt(true);
  Serial.print(F("LS Int Enable: "));
  Serial.println(apds.lsInterruptEnabled() ? F("[PASS]") : F("[FAIL]"));
  apds.enableLSInterrupt(false);

  apds.setLSIntChannel(APDS9999_INT_CH_RED);
  Serial.print(F("LS Int Channel RED: "));
  Serial.println(apds.getLSIntChannel() == APDS9999_INT_CH_RED ? F("[PASS]") : F("[FAIL]"));

  apds.setLSIntChannel(APDS9999_INT_CH_IR);
  Serial.print(F("LS Int Channel IR: "));
  Serial.println(apds.getLSIntChannel() == APDS9999_INT_CH_IR ? F("[PASS]") : F("[FAIL]"));

  // Interrupt persistence tests
  Serial.println(F("\n--- INT_PST Tests ---"));

  apds.setPSPersistence(5);
  Serial.print(F("PS Persistence 5: "));
  Serial.println(apds.getPSPersistence() == 5 ? F("[PASS]") : F("[FAIL]"));

  apds.setLSPersistence(10);
  Serial.print(F("LS Persistence 10: "));
  Serial.println(apds.getLSPersistence() == 10 ? F("[PASS]") : F("[FAIL]"));

  // PS Threshold tests
  Serial.println(F("\n--- PS Threshold Tests ---"));

  apds.setPSThresholdHigh(500);
  Serial.print(F("PS Thresh High 500: "));
  Serial.println(apds.getPSThresholdHigh() == 500 ? F("[PASS]") : F("[FAIL]"));

  apds.setPSThresholdLow(50);
  Serial.print(F("PS Thresh Low 50: "));
  Serial.println(apds.getPSThresholdLow() == 50 ? F("[PASS]") : F("[FAIL]"));

  // LS Threshold tests
  Serial.println(F("\n--- LS Threshold Tests ---"));

  apds.setLSThresholdHigh(100000);
  Serial.print(F("LS Thresh High 100000: "));
  Serial.println(apds.getLSThresholdHigh() == 100000 ? F("[PASS]") : F("[FAIL]"));

  apds.setLSThresholdLow(5000);
  Serial.print(F("LS Thresh Low 5000: "));
  Serial.println(apds.getLSThresholdLow() == 5000 ? F("[PASS]") : F("[FAIL]"));

  // Test max value (20-bit = 0xFFFFF = 1048575)
  apds.setLSThresholdHigh(1048575);
  Serial.print(F("LS Thresh High MAX: "));
  Serial.println(apds.getLSThresholdHigh() == 1048575 ? F("[PASS]") : F("[FAIL]"));

  // PS Cancellation tests
  Serial.println(F("\n--- PS Cancellation Tests ---"));

  apds.setPSCancellation(500);
  Serial.print(F("PS Digital Cancel 500: "));
  Serial.println(apds.getPSCancellation() == 500 ? F("[PASS]") : F("[FAIL]"));

  apds.setPSCancellation(2047);  // Max 11-bit
  Serial.print(F("PS Digital Cancel MAX: "));
  Serial.println(apds.getPSCancellation() == 2047 ? F("[PASS]") : F("[FAIL]"));

  apds.setPSAnalogCancellation(15);
  Serial.print(F("PS Analog Cancel 15: "));
  Serial.println(apds.getPSAnalogCancellation() == 15 ? F("[PASS]") : F("[FAIL]"));

  apds.setPSAnalogCancellation(31);  // Max 5-bit
  Serial.print(F("PS Analog Cancel MAX: "));
  Serial.println(apds.getPSAnalogCancellation() == 31 ? F("[PASS]") : F("[FAIL]"));

  // SAI mode tests
  Serial.println(F("\n--- SAI Mode Tests ---"));

  apds.setPSSleepAfterInterrupt(true);
  Serial.print(F("SAI_PS enable: "));
  Serial.println(apds.getPSSleepAfterInterrupt() ? F("[PASS]") : F("[FAIL]"));

  apds.setPSSleepAfterInterrupt(false);
  Serial.print(F("SAI_PS disable: "));
  Serial.println(!apds.getPSSleepAfterInterrupt() ? F("[PASS]") : F("[FAIL]"));

  apds.setLSSleepAfterInterrupt(true);
  Serial.print(F("SAI_LS enable: "));
  Serial.println(apds.getLSSleepAfterInterrupt() ? F("[PASS]") : F("[FAIL]"));

  apds.setLSSleepAfterInterrupt(false);
  Serial.print(F("SAI_LS disable: "));
  Serial.println(!apds.getLSSleepAfterInterrupt() ? F("[PASS]") : F("[FAIL]"));

  // Reset test - verify chip comes back
  Serial.println(F("\n--- Reset Test ---"));
  Serial.println(F("Testing reset..."));
  apds.reset();
  delay(20);  // Wait for reset
  // Try to read part ID to confirm chip is responsive
  Serial.print(F("Reset recovery: "));
  Serial.println(apds.getPartID() == 0x0C ? F("[PASS]") : F("[FAIL]"));

  // Live interrupt test
  Serial.println(F("\n--- Live Interrupt Test ---"));
  pinMode(INT_PIN, INPUT_PULLUP);
  apds.enableProximitySensor(true);
  apds.setPSThresholdHigh(50);  // Low threshold so hand triggers it
  apds.setPSPersistence(0);     // Trigger on first reading
  apds.enablePSInterrupt(true);
  delay(100);

  Serial.println(F("Wave hand near sensor NOW..."));
  bool triggered = false;
  for (int i = 0; i < 50; i++) {  // 5 second timeout
    if (digitalRead(INT_PIN) == LOW) {
      triggered = true;
      break;
    }
    delay(100);
  }

  if (triggered) {
    Serial.print(F("INT triggered! PS Int Status: "));
    Serial.println(apds.getPSInterruptStatus() ? F("SET [PASS]") : F("NOT SET [FAIL]"));
  } else {
    Serial.println(F("Timeout - no interrupt [FAIL]"));
  }

  apds.enablePSInterrupt(false);

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
