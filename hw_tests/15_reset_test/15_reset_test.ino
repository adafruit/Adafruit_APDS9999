/*!
 * @file 15_reset_test.ino
 * @brief Configure non-default settings, call reset(), verify defaults
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

  Serial.println("APDS9999 Reset Test");
  Serial.println("-------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  // Configure with non-default settings
  Serial.println("Setting non-default values...");

  apds.enableLightSensor(true);
  apds.enableProximitySensor(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_18X);
  apds.setLightResolution(APDS9999_LIGHT_RES_13BIT);
  apds.setLightMeasRate(APDS9999_LIGHT_RATE_2000MS);
  apds.setProxResolution(APDS9999_PROX_RES_11BIT);
  apds.setProxMeasRate(APDS9999_PROX_RATE_400MS);
  apds.setLEDPulses(128);
  apds.setProxThresholdHigh(1000);
  apds.setProxThresholdLow(500);
  apds.setProxPersistence(10);
  apds.setLightPersistence(10);

  // Verify settings were applied
  Serial.println("\nBefore reset:");
  Serial.print("  Light gain: ");
  Serial.println(apds.getLightGain());
  Serial.print("  Light resolution: ");
  Serial.println(apds.getLightResolution());
  Serial.print("  LED pulses: ");
  Serial.println(apds.getLEDPulses());
  Serial.print("  Prox threshold high: ");
  Serial.println(apds.getProxThresholdHigh());

  // Perform software reset
  Serial.println("\nPerforming software reset...");
  apds.reset();
  delay(100); // Allow reset to complete

  // Check if values returned to defaults
  Serial.println("\nAfter reset (checking defaults):");

  // Light gain default is 0x00 (1X)
  apds9999_light_gain_t gain = apds.getLightGain();
  Serial.print("  Light gain (expect 1X/0): ");
  Serial.print(gain);
  if (gain == APDS9999_LIGHT_GAIN_1X) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
  }

  // Light resolution default is 0x02 (18-bit)
  apds9999_light_resolution_t res = apds.getLightResolution();
  Serial.print("  Light resolution (expect 18bit/2): ");
  Serial.print(res);
  if (res == APDS9999_LIGHT_RES_18BIT) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
  }

  // LED pulses default is 0x01 (1 pulse)
  uint8_t pulses = apds.getLEDPulses();
  Serial.print("  LED pulses (expect 1): ");
  Serial.print(pulses);
  if (pulses == 1) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
  }

  // Prox threshold high default is 0x07FF (2047)
  uint16_t thresh = apds.getProxThresholdHigh();
  Serial.print("  Prox threshold high (expect 2047): ");
  Serial.print(thresh);
  if (thresh == 2047) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
  }

  // Prox persistence default is 0x00
  uint8_t pers = apds.getProxPersistence();
  Serial.print("  Prox persistence (expect 0): ");
  Serial.print(pers);
  if (pers == 0) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
  }

  // Light enabled default is false
  bool lightEn = apds.lightSensorEnabled();
  Serial.print("  Light enabled (expect false): ");
  Serial.print(lightEn ? "true" : "false");
  if (!lightEn) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
  }

  // Prox enabled default is false
  bool proxEn = apds.proximitySensorEnabled();
  Serial.print("  Prox enabled (expect false): ");
  Serial.print(proxEn ? "true" : "false");
  if (!proxEn) {
    Serial.println(" PASS");
    passed++;
  } else {
    Serial.println(" FAIL");
    failed++;
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
