/*
 * Simple proximity sensor example for APDS-9999
 */

#include "Adafruit_APDS9999.h"

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (!apds.begin()) {
    Serial.println("Failed to find APDS-9999");
    while (1) delay(10);
  }

  apds.enableProximitySensor(true);
  Serial.println("APDS-9999 Proximity Sensor");
}

void loop() {
  Serial.print("Proximity: ");
  Serial.println(apds.readProximity());
  delay(100);
}
