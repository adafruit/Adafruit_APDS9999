/*
 * Simple color sensor example for APDS-9999
 */

#include "Adafruit_APDS9999.h"

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  if (!apds.begin()) {
    Serial.println("Failed to find APDS-9999");
    while (1)
      delay(10);
  }

  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  Serial.println("APDS-9999 Color Sensor");
}

void loop() {
  uint32_t r, g, b, ir;

  if (apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.print("R:");
    Serial.print(r);
    Serial.print(" G:");
    Serial.print(g);
    Serial.print(" B:");
    Serial.print(b);
    Serial.print(" IR:");
    Serial.print(ir);
    Serial.print(" Lux:");
    Serial.println(apds.calculateLux(g));
  }

  delay(100);
}
