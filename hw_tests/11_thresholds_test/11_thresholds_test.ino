/*!
 * @file 11_thresholds_test.ino
 * @brief Write and read back all threshold registers
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

int passed = 0;
int failed = 0;

void testProxThresholds() {
  Serial.println("Testing Proximity Thresholds...\n");

  // Test values spanning the 11-bit range
  uint16_t test_values[] = {0, 100, 500, 1000, 2047};

  for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
    // Test high threshold
    apds.setProxThresholdHigh(test_values[i]);
    uint16_t readback = apds.getProxThresholdHigh();

    Serial.print("Prox High ");
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

    // Test low threshold
    apds.setProxThresholdLow(test_values[i]);
    readback = apds.getProxThresholdLow();

    Serial.print("Prox Low ");
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

void testLightThresholds() {
  Serial.println("\nTesting Light Thresholds...\n");

  // Test values spanning the 20-bit range
  uint32_t test_values[] = {0, 1000, 50000, 500000, 1048575};

  for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
    // Test high threshold
    apds.setLightThresholdHigh(test_values[i]);
    uint32_t readback = apds.getLightThresholdHigh();

    Serial.print("Light High ");
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

    // Test low threshold
    apds.setLightThresholdLow(test_values[i]);
    readback = apds.getLightThresholdLow();

    Serial.print("Light Low ");
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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Thresholds Test");
  Serial.println("------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  testProxThresholds();
  testLightThresholds();

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
