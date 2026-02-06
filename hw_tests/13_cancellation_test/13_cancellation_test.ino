/*!
 * @file 13_cancellation_test.ino
 * @brief Test prox cancellation (digital 0-65535, analog 0-7)
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

int passed = 0;
int failed = 0;

void testDigitalCancellation() {
  Serial.println("Testing Digital Cancellation (16-bit)...\n");

  // Test values spanning the 16-bit range
  uint16_t test_values[] = {0, 100, 1000, 10000, 32768, 65535};

  for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
    apds.setProxCancellation(test_values[i]);
    uint16_t readback = apds.getProxCancellation();

    Serial.print("Digital Cancel ");
    Serial.print(test_values[i]);
    Serial.print(": ");
    if (readback == test_values[i]) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }
}

void testAnalogCancellation() {
  Serial.println("\nTesting Analog Cancellation (0-7)...\n");

  for (uint8_t a = 0; a <= 7; a++) {
    apds.setProxAnalogCancellation(a);
    uint8_t readback = apds.getProxAnalogCancellation();

    Serial.print("Analog Cancel ");
    Serial.print(a);
    Serial.print(": ");
    if (readback == a) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }
}

void demonstrateCancellation() {
  Serial.println("\nDemonstrating cancellation effect on readings...\n");

  apds.enableProximitySensor(true);
  apds.setProxCancellation(0);
  apds.setProxAnalogCancellation(0);
  delay(200);

  uint16_t prox;
  apds.readProximity(&prox);
  Serial.print("Baseline (no cancellation): ");
  Serial.println(prox);

  // Apply some digital cancellation
  apds.setProxCancellation(100);
  delay(200);
  apds.readProximity(&prox);
  Serial.print("With digital cancel=100: ");
  Serial.println(prox);

  // Reset
  apds.setProxCancellation(0);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Cancellation Test");
  Serial.println("--------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  testDigitalCancellation();
  testAnalogCancellation();
  demonstrateCancellation();

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
