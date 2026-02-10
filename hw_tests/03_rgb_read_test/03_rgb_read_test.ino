/*!
 * @file 03_rgb_read_test.ino
 * @brief Enable light sensor in RGB mode, read RGB+IR continuously
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 RGB Read Test");
  Serial.println("----------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable light sensor in RGB mode
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);

  Serial.println("\nReading RGB + IR values (shine colored lights to test):\n");
}

void loop() {
  uint32_t r, g, b, ir;

  if (apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.print("R: ");
    Serial.print(r);
    Serial.print("\tG: ");
    Serial.print(g);
    Serial.print("\tB: ");
    Serial.print(b);
    Serial.print("\tIR: ");
    Serial.print(ir);

    // Calculate approximate lux
    float lux = apds.calculateLux(g);
    Serial.print("\tLux: ");
    Serial.println(lux, 1);
  } else {
    Serial.println("Read error!");
  }

  delay(200);
}
