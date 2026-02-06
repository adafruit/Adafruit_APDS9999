/*!
 * @file 12_persistence_test.ino
 * @brief Test persistence: register roundtrip + functional timing with servo
 *
 * Hardware setup:
 * - Servo on A0 (0°=away, 105°=close to sensor)
 * - INT pin on D2
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

Adafruit_APDS9999 apds;
Servo myServo;

#define SERVO_PIN A0
#define INT_PIN 2
#define SERVO_AWAY 0
#define SERVO_CLOSE 105

volatile bool interruptFired = false;

int passed = 0;
int failed = 0;

void interruptHandler() {
  interruptFired = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Persistence Test");
  Serial.println("=========================");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  // ========================================
  // Phase 1: Register Roundtrip Tests
  // ========================================
  Serial.println("Phase 1: Register Roundtrip Tests");
  Serial.println("----------------------------------");

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
  Serial.print("Phase 1 Results - Passed: ");
  Serial.print(passed);
  Serial.print(" / Failed: ");
  Serial.println(failed);

  // ========================================
  // Phase 2: Functional Persistence Timing Test
  // ========================================
  Serial.println("\nPhase 2: Functional Persistence Timing Test");
  Serial.println("--------------------------------------------");

  // Setup servo
  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_AWAY);
  delay(1000);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), interruptHandler, FALLING);

  // Enable proximity sensor
  apds.enableProximitySensor(true);
  delay(200);

  // Calibrate: read baseline at away position
  Serial.println("Calibrating...");
  Serial.print("Servo at ");
  Serial.print(SERVO_AWAY);
  Serial.println(" degrees (away)");
  delay(500);

  uint16_t proxBaseline = 0;
  uint16_t proxReading;
  for (int i = 0; i < 5; i++) {
    apds.readProximity(&proxReading);
    proxBaseline += proxReading;
    delay(100);
  }
  proxBaseline /= 5;
  Serial.print("Prox baseline: ");
  Serial.println(proxBaseline);

  // Move servo close and read high value
  myServo.write(SERVO_CLOSE);
  Serial.print("Servo at ");
  Serial.print(SERVO_CLOSE);
  Serial.println(" degrees (close)");
  delay(1000);

  uint16_t proxHigh = 0;
  for (int i = 0; i < 5; i++) {
    apds.readProximity(&proxReading);
    proxHigh += proxReading;
    delay(100);
  }
  proxHigh /= 5;
  Serial.print("Prox high: ");
  Serial.println(proxHigh);

  // Calculate threshold at 50% between baseline and high
  uint16_t threshold = (proxBaseline + proxHigh) / 2;
  Serial.print("Threshold (50%): ");
  Serial.println(threshold);

  // Return servo to away position
  myServo.write(SERVO_AWAY);
  delay(1000);

  // Check we have enough range
  if (proxHigh <= proxBaseline + 50) {
    Serial.println("FAIL: Insufficient prox range for timing test");
    failed++;
  } else {
    // Test persistence values 1 and 8
    uint8_t testPersistence[] = {1, 8};
    unsigned long timingResults[2] = {0, 0};
    bool timingSuccess[2] = {false, false};

    for (int t = 0; t < 2; t++) {
      uint8_t pers = testPersistence[t];
      Serial.print("\nTesting persistence = ");
      Serial.println(pers);

      // Set prox high threshold
      apds.setProxThresholdHigh(threshold);

      // Set persistence
      apds.setProxPersistence(pers);

      // Enable prox interrupt
      apds.enableProxInterrupt(true);

      // Ensure servo is away (below threshold)
      myServo.write(SERVO_AWAY);
      delay(500);

      // Clear any pending interrupt by reading status
      apds.getMainStatus();
      interruptFired = false;

      // Move servo to close position and start timing
      unsigned long startTime = millis();
      myServo.write(SERVO_CLOSE);

      // Wait for interrupt with 3s timeout
      unsigned long timeout = 3000;
      while (!interruptFired && (millis() - startTime) < timeout) {
        delay(1);
      }
      unsigned long endTime = millis();

      unsigned long elapsed = endTime - startTime;
      timingResults[t] = elapsed;

      if (interruptFired) {
        Serial.print("Interrupt fired after ");
        Serial.print(elapsed);
        Serial.println(" ms");
        timingSuccess[t] = true;
      } else {
        Serial.print("TIMEOUT after ");
        Serial.print(elapsed);
        Serial.println(" ms - no interrupt");
        timingSuccess[t] = false;
      }

      // Disable interrupt and return servo
      apds.enableProxInterrupt(false);
      apds.getMainStatus(); // Clear status
      interruptFired = false;
      myServo.write(SERVO_AWAY);
      delay(500);
    }

    // Evaluate results
    Serial.println("\n--- Timing Test Results ---");
    Serial.print("Persistence 1: ");
    if (timingSuccess[0]) {
      Serial.print(timingResults[0]);
      Serial.println(" ms");
    } else {
      Serial.println("TIMEOUT");
    }

    Serial.print("Persistence 8: ");
    if (timingSuccess[1]) {
      Serial.print(timingResults[1]);
      Serial.println(" ms");
    } else {
      Serial.println("TIMEOUT");
    }

    // Pass criteria
    if (timingSuccess[0] && timingSuccess[1]) {
      Serial.println("Both persistence values triggered interrupt: PASS");
      passed++;

      // Check that persistence 8 took longer than persistence 1 by at least
      // 200ms
      if (timingResults[1] > timingResults[0] + 200) {
        Serial.print("Persistence 8 took ");
        Serial.print(timingResults[1] - timingResults[0]);
        Serial.println(" ms longer than persistence 1: PASS");
        passed++;
      } else {
        Serial.print("Persistence 8 only took ");
        Serial.print((long)timingResults[1] - (long)timingResults[0]);
        Serial.println(" ms longer (expected >200ms): FAIL");
        failed++;
      }
    } else {
      Serial.println("Not all persistence values triggered interrupt: FAIL");
      failed++;
      if (!timingSuccess[0])
        failed++;
      if (!timingSuccess[1])
        failed++;
    }
  }

  // Cleanup
  Serial.println("\nCleanup...");
  myServo.write(SERVO_AWAY);
  delay(500);
  myServo.detach();
  detachInterrupt(digitalPinToInterrupt(INT_PIN));

  // Final results
  Serial.println("\n=============================");
  Serial.println("Final Combined Results");
  Serial.println("=============================");
  Serial.print("Passed: ");
  Serial.print(passed);
  Serial.print(" / Failed: ");
  Serial.println(failed);
  Serial.println(failed == 0 ? "*** TEST PASSED ***" : "*** TEST FAILED ***");
}

void loop() {
  delay(1000);
}
