/*!
 * @file 02_prox_read_test.ino
 * @brief Enable proximity sensor, read continuously, wave hand to see values
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Proximity Read Test");
  Serial.println("----------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable proximity sensor
  apds.enableProximitySensor(true);

  Serial.println("\nWave hand near sensor to see values change!");
  Serial.println("Proximity values (higher = closer):\n");
}

void loop() {
  uint16_t prox;
  bool overflow;

  if (apds.readProximity(&prox, &overflow)) {
    Serial.print("Proximity: ");
    Serial.print(prox);
    if (overflow) {
      Serial.print(" [OVERFLOW]");
    }

    // Visual bar
    Serial.print("  |");
    int bars = map(prox, 0, 2047, 0, 40);
    for (int i = 0; i < bars; i++) {
      Serial.print("=");
    }
    Serial.println();
  } else {
    Serial.println("Read error!");
  }

  delay(100);
}
