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
}

void loop() {
  // Nothing here yet
  delay(100);
}
