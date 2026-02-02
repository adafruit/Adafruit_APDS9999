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
}

void loop() {
  // Nothing here yet
  delay(100);
}
