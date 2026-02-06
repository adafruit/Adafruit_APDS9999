/*!
 * @file 20_servo_sleep_after_int_test.ino
 * @brief Test Sleep-After-Interrupt (SAI) modes for proximity and light sensors
 * @note Connect INT pin to D2, Servo to A0
 *       Servo 90° = paper in front of sensor (trigger prox)
 *       Servo 0° = paper away from sensor
 *
 * SAI mode puts sensor into standby after an interrupt fires.
 * Useful for power saving - sensor sleeps until MCU acknowledges by reading
 * status.
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

#define INT_PIN 2
#define SERVO_PIN A0

Adafruit_APDS9999 apds;
Servo testServo;

volatile bool intFired = false;

void isr() {
  intFired = true;
}

/**
 * @brief Move servo to specified angle
 * @param angle 90 = paper in front of sensor, 0 = paper away
 */
void moveServo(int angle) {
  testServo.write(angle);
  delay(500); // Allow servo to reach position
}

/**
 * @brief Check if proximity readings are updating
 * @return true if readings change over time, false if stuck (standby)
 */
bool proxReadingsUpdating() {
  uint16_t readings[5];

  // Take 5 readings with delays
  for (int i = 0; i < 5; i++) {
    apds.readProximity(&readings[i]);
    delay(50);
  }

  // Check if at least 2 different values exist (readings are updating)
  for (int i = 1; i < 5; i++) {
    if (readings[i] != readings[0]) {
      return true; // Found variation = updating
    }
  }
  return false; // All same = likely in standby
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Sleep-After-Interrupt Test");
  Serial.println("====================================");
  Serial.println("Hardware: INT=D2, Servo=A0");
  Serial.println();

  // Initialize servo
  testServo.attach(SERVO_PIN);
  moveServo(0);
  Serial.println("Servo initialized at 0deg (away)");

  // Initialize sensor
  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("APDS9999 found!");
  Serial.println();

  // =====================================================
  // PART A: Verify getter/setter for SAI modes
  // =====================================================
  Serial.println("=== PART A: Getter/Setter Verification ===");
  Serial.println();
  int passCount = 0;
  int failCount = 0;

  // Test 1: setProxSleepAfterInterrupt(false) -> get returns false
  apds.setProxSleepAfterInterrupt(false);
  bool val = apds.getProxSleepAfterInterrupt();
  Serial.print("Test A1: setProxSAI(false) -> get: ");
  Serial.print(val ? "true" : "false");
  if (!val) {
    Serial.println(" PASS");
    passCount++;
  } else {
    Serial.println(" FAIL");
    failCount++;
  }

  // Test 2: setProxSleepAfterInterrupt(true) -> get returns true
  apds.setProxSleepAfterInterrupt(true);
  val = apds.getProxSleepAfterInterrupt();
  Serial.print("Test A2: setProxSAI(true) -> get: ");
  Serial.print(val ? "true" : "false");
  if (val) {
    Serial.println(" PASS");
    passCount++;
  } else {
    Serial.println(" FAIL");
    failCount++;
  }

  // Test 3: setLightSleepAfterInterrupt(false) -> get returns false
  apds.setLightSleepAfterInterrupt(false);
  val = apds.getLightSleepAfterInterrupt();
  Serial.print("Test A3: setLightSAI(false) -> get: ");
  Serial.print(val ? "true" : "false");
  if (!val) {
    Serial.println(" PASS");
    passCount++;
  } else {
    Serial.println(" FAIL");
    failCount++;
  }

  // Test 4: setLightSleepAfterInterrupt(true) -> get returns true
  apds.setLightSleepAfterInterrupt(true);
  val = apds.getLightSleepAfterInterrupt();
  Serial.print("Test A4: setLightSAI(true) -> get: ");
  Serial.print(val ? "true" : "false");
  if (val) {
    Serial.println(" PASS");
    passCount++;
  } else {
    Serial.println(" FAIL");
    failCount++;
  }

  Serial.println();
  Serial.print("Part A Summary: ");
  Serial.print(passCount);
  Serial.print("/4 passed");
  if (failCount > 0) {
    Serial.print(", ");
    Serial.print(failCount);
    Serial.print(" failed");
  }
  Serial.println();
  Serial.println();

  // =====================================================
  // PART B: Prox SAI Behavior Test
  // =====================================================
  Serial.println("=== PART B: Prox SAI Behavior Test ===");
  Serial.println();

  // Reset sensor state
  apds.reset();
  delay(100);

  // Re-initialize after reset
  if (!apds.begin()) {
    Serial.println("FAIL: Sensor lost after reset!");
    while (1) {
      delay(10);
    }
  }

  // Configure proximity with SAI enabled
  apds.enableProximitySensor(true);
  apds.setProxSleepAfterInterrupt(true);
  apds.setProxThresholdLow(10);
  apds.setProxThresholdHigh(20);
  apds.setProxPersistence(1);
  apds.enableProxInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending status
  apds.getMainStatus();
  intFired = false;

  Serial.println("Config: Prox enabled, SAI=true, thresh low=10, high=20");
  Serial.println();

  // Step B1: Verify readings update before interrupt
  Serial.println("Step B1: Verify readings update (before interrupt)");
  moveServo(0); // Ensure paper away
  delay(500);

  uint16_t prox1, prox2;
  apds.readProximity(&prox1);
  delay(200);
  apds.readProximity(&prox2);
  Serial.print("  Prox reading 1: ");
  Serial.println(prox1);
  Serial.print("  Prox reading 2: ");
  Serial.println(prox2);
  Serial.print("  Data valid: ");
  Serial.println((prox1 < 1000 && prox2 < 1000) ? "PASS" : "FAIL");
  if (prox1 < 1000 && prox2 < 1000)
    passCount++;
  else
    failCount++;
  Serial.println();

  // Step B2: Trigger interrupt with servo
  Serial.println("Step B2: Trigger interrupt (servo to 90deg)");
  moveServo(90);
  delay(1000); // Wait for interrupt to fire

  bool intTriggered = intFired;
  Serial.print("  Interrupt fired: ");
  Serial.println(intTriggered ? "YES (PASS)" : "NO (FAIL)");
  if (intTriggered)
    passCount++;
  else
    failCount++;
  Serial.println();

  // Step B3: Check readings stop updating (sensor in standby)
  Serial.println("Step B3: Check sensor enters standby after interrupt");

  // Take multiple readings - they should be the same (sensor in standby)
  uint16_t standbyReadings[5];
  for (int i = 0; i < 5; i++) {
    apds.readProximity(&standbyReadings[i]);
    Serial.print("  Standby read ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(standbyReadings[i]);
    delay(100);
  }

  // Check if readings are all similar (indicating standby mode)
  bool allSimilar = true;
  for (int i = 1; i < 5; i++) {
    // Allow small variance (noise) but readings should be very close
    int diff = abs((int)standbyReadings[i] - (int)standbyReadings[0]);
    if (diff > 5) {
      allSimilar = false;
      break;
    }
  }
  Serial.print("  Readings frozen (standby): ");
  Serial.println(allSimilar ? "PASS" : "FAIL");
  if (allSimilar)
    passCount++;
  else
    failCount++;
  Serial.println();

  // Step B4: Clear interrupt by reading status
  Serial.println("Step B4: Clear interrupt (read status)");
  uint8_t status = apds.getMainStatus();
  Serial.print("  Status read: 0x");
  Serial.println(status, HEX);
  intFired = false;
  Serial.println("  Interrupt cleared");
  Serial.println();

  // Step B5: Verify readings resume
  Serial.println("Step B5: Verify readings resume after clearing interrupt");
  moveServo(0); // Move paper away to change prox value
  delay(500);

  // Take readings after clearing - should update now
  uint16_t resumeReadings[3];
  for (int i = 0; i < 3; i++) {
    apds.readProximity(&resumeReadings[i]);
    Serial.print("  Resume read ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(resumeReadings[i]);
    delay(200);
  }

  // Readings should be different from standby readings (sensor resumed)
  // Also, with paper away, they should be lower
  bool resumed = (resumeReadings[0] < standbyReadings[0] - 10) ||
                 (resumeReadings[1] < standbyReadings[0] - 10) ||
                 (resumeReadings[2] < standbyReadings[0] - 10);
  Serial.print("  Readings resumed: ");
  Serial.println(resumed ? "PASS" : "FAIL");
  if (resumed)
    passCount++;
  else
    failCount++;

  // =====================================================
  // FINAL SUMMARY
  // =====================================================
  Serial.println();
  Serial.println("=== FINAL SUMMARY ===");
  Serial.print("Total tests: ");
  Serial.println(passCount + failCount);
  Serial.print("Passed: ");
  Serial.println(passCount);
  Serial.print("Failed: ");
  Serial.println(failCount);
  Serial.println();

  if (failCount == 0) {
    Serial.println("OVERALL: PASS - All Sleep-After-Interrupt tests passed!");
  } else {
    Serial.println("OVERALL: FAIL - Some tests did not pass");
  }

  Serial.println();
  Serial.println("--- TEST COMPLETE ---");

  // Return servo to rest position
  moveServo(0);
}

void loop() {
  // Test runs once in setup
  delay(1000);
}
