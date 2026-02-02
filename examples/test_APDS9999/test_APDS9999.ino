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
