/*
 * Full test sketch for APDS-9999 Digital Proximity and RGB Sensor
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 *
 * Displays continuous data from both proximity and light sensors.
 */

#include "Adafruit_APDS9999.h"

Adafruit_APDS9999 apds = Adafruit_APDS9999();

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("APDS-9999 Full Test"));
  Serial.println(F("==================="));

  if (!apds.begin()) {
    Serial.println(F("Couldn't find APDS-9999 chip"));
    while (1)
      delay(10);
  }

  // Print chip info
  Serial.print(F("I2C Address: 0x"));
  Serial.println(APDS9999_I2CADDR_DEFAULT, HEX);
  Serial.print(F("Part ID: 0x"));
  Serial.print(apds.getPartID(), HEX);
  Serial.print(F(" Revision: 0x"));
  Serial.println(apds.getRevisionID(), HEX);

  // Check power-on status (should be set after fresh start)
  uint8_t status = apds.getMainStatus();
  Serial.print(F("Power-on status: "));
  Serial.println((status & APDS9999_STATUS_POWER_ON) ? F("Yes") : F("No"));

  // Test soft reset
  Serial.print(F("Testing soft reset... "));
  apds.reset();
  delay(10); // Wait for reset to complete
  // Re-init I2C device after reset
  apds.begin();
  status = apds.getMainStatus();
  Serial.println((status & APDS9999_STATUS_POWER_ON)
                     ? F("OK (power-on detected)")
                     : F("FAIL"));

  // === Light Sensor Configuration ===
  Serial.println(F("\n--- Light Sensor Configuration ---"));

  apds.enableLightSensor(true);
  Serial.print(F("Enabled: "));
  Serial.println(apds.lightSensorEnabled() ? F("Yes") : F("No"));

  apds.setRGBMode(true);
  Serial.print(F("Mode: "));
  Serial.println(apds.getRGBMode() ? F("RGB + IR") : F("Ambient Light + IR"));

  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);
  Serial.print(F("Gain: "));
  switch (apds.getLightGain()) {
    case APDS9999_LIGHT_GAIN_1X:
      Serial.println(F("1x"));
      break;
    case APDS9999_LIGHT_GAIN_3X:
      Serial.println(F("3x"));
      break;
    case APDS9999_LIGHT_GAIN_6X:
      Serial.println(F("6x"));
      break;
    case APDS9999_LIGHT_GAIN_9X:
      Serial.println(F("9x"));
      break;
    case APDS9999_LIGHT_GAIN_18X:
      Serial.println(F("18x"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  apds.setLightResolution(APDS9999_LIGHT_RES_18BIT);
  Serial.print(F("Resolution: "));
  switch (apds.getLightResolution()) {
    case APDS9999_LIGHT_RES_20BIT:
      Serial.println(F("20-bit (400ms conversion)"));
      break;
    case APDS9999_LIGHT_RES_19BIT:
      Serial.println(F("19-bit (200ms conversion)"));
      break;
    case APDS9999_LIGHT_RES_18BIT:
      Serial.println(F("18-bit (100ms conversion)"));
      break;
    case APDS9999_LIGHT_RES_17BIT:
      Serial.println(F("17-bit (50ms conversion)"));
      break;
    case APDS9999_LIGHT_RES_16BIT:
      Serial.println(F("16-bit (25ms conversion)"));
      break;
    case APDS9999_LIGHT_RES_13BIT:
      Serial.println(F("13-bit (3ms conversion)"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  apds.setLightMeasRate(APDS9999_LIGHT_RATE_100MS);
  Serial.print(F("Measurement Rate: "));
  switch (apds.getLightMeasRate()) {
    case APDS9999_LIGHT_RATE_25MS:
      Serial.println(F("25ms"));
      break;
    case APDS9999_LIGHT_RATE_50MS:
      Serial.println(F("50ms"));
      break;
    case APDS9999_LIGHT_RATE_100MS:
      Serial.println(F("100ms"));
      break;
    case APDS9999_LIGHT_RATE_200MS:
      Serial.println(F("200ms"));
      break;
    case APDS9999_LIGHT_RATE_500MS:
      Serial.println(F("500ms"));
      break;
    case APDS9999_LIGHT_RATE_1000MS:
      Serial.println(F("1000ms"));
      break;
    case APDS9999_LIGHT_RATE_2000MS:
      Serial.println(F("2000ms"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  // === Proximity Sensor Configuration ===
  Serial.println(F("\n--- Proximity Sensor Configuration ---"));

  apds.enableProximitySensor(true);
  Serial.print(F("Enabled: "));
  Serial.println(apds.proximitySensorEnabled() ? F("Yes") : F("No"));

  apds.setProxResolution(APDS9999_PROX_RES_11BIT);
  Serial.print(F("Resolution: "));
  switch (apds.getProxResolution()) {
    case APDS9999_PROX_RES_8BIT:
      Serial.println(F("8-bit"));
      break;
    case APDS9999_PROX_RES_9BIT:
      Serial.println(F("9-bit"));
      break;
    case APDS9999_PROX_RES_10BIT:
      Serial.println(F("10-bit"));
      break;
    case APDS9999_PROX_RES_11BIT:
      Serial.println(F("11-bit"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  apds.setProxMeasRate(APDS9999_PROX_RATE_100MS);
  Serial.print(F("Measurement Rate: "));
  switch (apds.getProxMeasRate()) {
    case APDS9999_PROX_RATE_6MS:
      Serial.println(F("6.25ms"));
      break;
    case APDS9999_PROX_RATE_12MS:
      Serial.println(F("12.5ms"));
      break;
    case APDS9999_PROX_RATE_25MS:
      Serial.println(F("25ms"));
      break;
    case APDS9999_PROX_RATE_50MS:
      Serial.println(F("50ms"));
      break;
    case APDS9999_PROX_RATE_100MS:
      Serial.println(F("100ms"));
      break;
    case APDS9999_PROX_RATE_200MS:
      Serial.println(F("200ms"));
      break;
    case APDS9999_PROX_RATE_400MS:
      Serial.println(F("400ms"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  apds.setLEDCurrent(APDS9999_LED_CURRENT_25MA);
  Serial.print(F("LED Current: "));
  switch (apds.getLEDCurrent()) {
    case APDS9999_LED_CURRENT_10MA:
      Serial.println(F("10mA"));
      break;
    case APDS9999_LED_CURRENT_25MA:
      Serial.println(F("25mA"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  apds.setLEDFrequency(APDS9999_LED_FREQ_60KHZ);
  Serial.print(F("LED Frequency: "));
  switch (apds.getLEDFrequency()) {
    case APDS9999_LED_FREQ_60KHZ:
      Serial.println(F("60kHz"));
      break;
    case APDS9999_LED_FREQ_70KHZ:
      Serial.println(F("70kHz"));
      break;
    case APDS9999_LED_FREQ_80KHZ:
      Serial.println(F("80kHz"));
      break;
    case APDS9999_LED_FREQ_90KHZ:
      Serial.println(F("90kHz"));
      break;
    case APDS9999_LED_FREQ_100KHZ:
      Serial.println(F("100kHz"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  apds.setLEDPulses(8);
  Serial.print(F("LED Pulses: "));
  Serial.println(apds.getLEDPulses());

  // === Start continuous reading ===
  Serial.println(F("\n--- Sensor Readings ---\n"));

  delay(200); // Let sensors stabilize
}

void loop() {
  bool printed = false;

  // Read status once - reading clears ALL status bits!
  uint8_t status = apds.getMainStatus();

  // Read proximity when ready
  if (status & APDS9999_STATUS_PROX_DATA) {
    uint16_t prox;
    bool overflow;
    if (apds.readProximity(&prox, &overflow)) {
      Serial.print(F("Proximity: "));
      if (overflow) {
        Serial.print(F("OVERFLOW"));
      } else {
        Serial.print(prox);
      }
      printed = true;
    }
  }

  // Read RGBIR when ready
  if (status & APDS9999_STATUS_LIGHT_DATA) {
    uint32_t r, g, b, ir;
    if (apds.getRGBIRData(&r, &g, &b, &ir)) {
      if (printed)
        Serial.print(F("\t"));
      Serial.print(F("Red: "));
      Serial.print(r);
      Serial.print(F("\tGreen: "));
      Serial.print(g);
      Serial.print(F("\tBlue: "));
      Serial.print(b);
      Serial.print(F("\tIR: "));
      Serial.print(ir);
      printed = true;
    }
  }

  if (printed) {
    Serial.println();
  }

  delay(50);
}
