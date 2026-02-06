/*!
 * @file 12_persistence_test.ino
 * @brief Test all persistence values (0-15) for prox and light
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

int passed = 0;
int failed = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Persistence Test");
  Serial.println("-------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  Serial.println("Testing Proximity Persistence (0-15)...\n");

  for (uint8_t p = 0; p <= 15; p++) {
    apds.setProxPersistence(p);
    uint8_t readback = apds.getProxPersistence();

    Serial.print("Prox Persistence ");
    Serial.print(p);
    Serial.print(": ");
    if (readback == p) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }

  Serial.println("\nTesting Light Persistence (0-15)...\n");

  for (uint8_t p = 0; p <= 15; p++) {
    apds.setLightPersistence(p);
    uint8_t readback = apds.getLightPersistence();

    Serial.print("Light Persistence ");
    Serial.print(p);
    Serial.print(": ");
    if (readback == p) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }

  Serial.println();
  Serial.print("Passed: ");
  Serial.print(passed);
  Serial.print(" / Failed: ");
  Serial.println(failed);
  Serial.println(failed == 0 ? "*** TEST PASSED ***" : "*** TEST FAILED ***");
}

void loop() {
  delay(1000);
}
