/*!
 * @file 01_begin_test.ino
 * @brief Initialize sensor, print part ID (expect 0xC2) and revision ID
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Begin Test");
  Serial.println("-------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  uint8_t partID = apds.getPartID();
  uint8_t revID = apds.getRevisionID();

  Serial.print("Part ID: 0x");
  Serial.print(partID, HEX);
  Serial.print(" (expected 0xC2) - ");
  Serial.println((partID == 0xC2) ? "PASS" : "FAIL");

  Serial.print("Revision ID: 0x");
  Serial.println(revID, HEX);

  if (partID == 0xC2) {
    Serial.println("\n*** TEST PASSED ***");
  } else {
    Serial.println("\n*** TEST FAILED ***");
  }
}

void loop() {
  delay(1000);
}
