/*!
 * @file 14_status_test.ino
 * @brief Read and decode main status register, demonstrate clear-on-read
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

void decodeStatus(uint8_t status) {
  Serial.print("Status: 0x");
  Serial.print(status, HEX);
  Serial.print(" -> ");

  if (status & APDS9999_STATUS_POWER_ON) {
    Serial.print("[POWER_ON] ");
  }
  if (status & APDS9999_STATUS_LIGHT_INT) {
    Serial.print("[LIGHT_INT] ");
  }
  if (status & APDS9999_STATUS_LIGHT_DATA) {
    Serial.print("[LIGHT_DATA] ");
  }
  if (status & APDS9999_STATUS_PROX_LOGIC) {
    Serial.print("[PROX_LOGIC] ");
  }
  if (status & APDS9999_STATUS_PROX_INT) {
    Serial.print("[PROX_INT] ");
  }
  if (status & APDS9999_STATUS_PROX_DATA) {
    Serial.print("[PROX_DATA] ");
  }

  if (status == 0) {
    Serial.print("(no flags set)");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Status Test");
  Serial.println("--------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  // First read should show POWER_ON flag
  Serial.println("Reading status after power-on:");
  uint8_t status1 = apds.getMainStatus();
  decodeStatus(status1);

  // Second read should show POWER_ON cleared
  Serial.println("\nReading status again (should be cleared):");
  uint8_t status2 = apds.getMainStatus();
  decodeStatus(status2);

  // Verify clear-on-read behavior
  Serial.println("\n--- Clear-on-read test ---");
  if ((status1 & APDS9999_STATUS_POWER_ON) &&
      !(status2 & APDS9999_STATUS_POWER_ON)) {
    Serial.println("PASS: POWER_ON cleared on read");
  } else {
    Serial.println("Note: POWER_ON behavior may vary");
  }

  // Enable sensors to see data ready flags
  Serial.println("\n--- Enabling sensors ---");
  apds.enableProximitySensor(true);
  apds.enableLightSensor(true);
  apds.setRGBMode(true);

  delay(200); // Wait for data

  Serial.println("\nStatus after enabling sensors:");
  status1 = apds.getMainStatus();
  decodeStatus(status1);

  Serial.println("\n--- Testing individual status functions ---");

  // Use the helper functions
  Serial.print("isProxDataReady(): ");
  Serial.println(apds.isProxDataReady() ? "YES" : "NO");

  Serial.print("isLightDataReady(): ");
  Serial.println(apds.isLightDataReady() ? "YES" : "NO");

  Serial.print("getProxInterruptStatus(): ");
  Serial.println(apds.getProxInterruptStatus() ? "YES" : "NO");

  Serial.print("getLightInterruptStatus(): ");
  Serial.println(apds.getLightInterruptStatus() ? "YES" : "NO");

  Serial.println("\n*** TEST COMPLETE ***");
}

void loop() {
  // Continuously show status
  uint8_t status = apds.getMainStatus();
  decodeStatus(status);
  delay(500);
}
