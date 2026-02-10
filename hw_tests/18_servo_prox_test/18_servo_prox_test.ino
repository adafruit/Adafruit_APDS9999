/*!
 * @file 18_servo_prox_test.ino
 * @brief Automated proximity interrupt test with self-calibrating thresholds
 * @note Connect INT pin to D2, Servo to A0
 *       Servo 105° = paper in front of sensor
 *       Servo 0° = paper away from sensor
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

#define INT_PIN 2
#define SERVO_PIN A0
#define NUM_SAMPLES 5
#define INT_TIMEOUT_MS 2000

Adafruit_APDS9999 apds;
Servo testServo;

volatile bool intFired = false;

void isr() {
  intFired = true;
}

/**
 * @brief Move servo to specified angle
 * @param angle 105 = paper in front of sensor, 0 = paper away
 */
void moveServo(int angle) {
  testServo.write(angle);
  delay(600); // Allow servo to reach position
}

/**
 * @brief Read proximity multiple times and return average
 */
uint16_t readProxAverage(int samples) {
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++) {
    uint16_t prox;
    apds.readProximity(&prox);
    sum += prox;
    delay(50);
  }
  return (uint16_t)(sum / samples);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Servo Proximity Interrupt Test");
  Serial.println("========================================");
  Serial.println("Hardware: INT=D2, Servo=A0");
  Serial.println("Self-calibrating thresholds");
  Serial.println();

  // Initialize servo
  testServo.attach(SERVO_PIN);
  moveServo(0); // Start with paper away
  Serial.println("Servo initialized at 0deg (away)");

  // Initialize sensor
  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }
  Serial.println("APDS9999 found!");

  // Full reset and clean init (same as other working tests)
  apds.reset();
  delay(200);
  apds.begin();

  // Disable light sensor, enable prox only
  apds.enableLightSensor(false);
  apds.enableProximitySensor(true);

  // Good defaults for reliable readings
  apds.setLEDPulses(32);
  apds.setLEDCurrent(APDS9999_LED_CURRENT_25MA);

  // ========================================
  // PHASE 1: CALIBRATION
  // ========================================
  Serial.println();
  Serial.println("--- PHASE 1: CALIBRATION ---");

  // Calibrate baseline (servo away)
  Serial.println("Calibrating baseline (servo at 0deg)...");
  moveServo(0);
  delay(500);
  uint16_t baseline = readProxAverage(NUM_SAMPLES);
  Serial.print("  Baseline (away): ");
  Serial.println(baseline);

  // Calibrate high value (servo close)
  Serial.println("Calibrating high value (servo at 105deg)...");
  moveServo(105);
  delay(500);
  uint16_t highVal = readProxAverage(NUM_SAMPLES);
  Serial.print("  High value (close): ");
  Serial.println(highVal);

  // Calculate threshold at 50% between baseline and high
  uint16_t threshold = baseline + ((highVal - baseline) / 2);
  Serial.print("  Calculated threshold (50%): ");
  Serial.println(threshold);

  // Sanity check
  if (highVal <= baseline + 10) {
    Serial.println("FAIL: High value not significantly greater than baseline!");
    Serial.println("      Check servo/paper positioning.");
    moveServo(0);
    testServo.detach();
    while (1) {
      delay(10);
    }
  }

  // ========================================
  // PHASE 2: INTERRUPT TEST
  // ========================================
  Serial.println();
  Serial.println("--- PHASE 2: INTERRUPT TEST ---");

  // Move servo away first
  Serial.println("Moving servo to 0deg (away)...");
  moveServo(0);
  delay(500);

  // Configure thresholds with calibrated value
  apds.setProxThresholdLow(0);
  apds.setProxThresholdHigh(threshold);
  Serial.print("Prox threshold high set to: ");
  Serial.println(threshold);

  // Set persistence to 1 for fast response
  apds.setProxPersistence(1);

  // Enable proximity interrupt
  apds.enableProxInterrupt(true);
  Serial.println("Prox interrupt enabled");

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending interrupt
  apds.getMainStatus();
  intFired = false;
  Serial.println("Interrupt cleared, waiting for trigger...");

  // Move servo to trigger position
  Serial.println();
  Serial.println(
      "Moving servo to 105deg (close) - should trigger interrupt...");
  moveServo(105);

  // Wait for interrupt with timeout
  unsigned long startTime = millis();
  while (!intFired && (millis() - startTime < INT_TIMEOUT_MS)) {
    delay(10);
  }

  // Read final proximity value
  uint16_t finalProx;
  apds.readProximity(&finalProx);

  // Get status
  uint8_t status = apds.getMainStatus();
  bool intStatus = intFired;

  Serial.print("  Final proximity: ");
  Serial.println(finalProx);
  Serial.print("  Interrupt fired: ");
  Serial.println(intStatus ? "YES" : "NO");
  Serial.print("  Status register: 0x");
  Serial.println(status, HEX);
  Serial.print("  Time waited: ");
  Serial.print(millis() - startTime);
  Serial.println(" ms");

  // ========================================
  // CLEANUP & RESULTS
  // ========================================
  Serial.println();
  Serial.println("--- CLEANUP ---");
  moveServo(0);
  testServo.detach();
  Serial.println("Servo returned to 0deg and detached");

  Serial.println();
  Serial.println("--- TEST RESULTS ---");
  Serial.print("Baseline (away):      ");
  Serial.println(baseline);
  Serial.print("High value (close):   ");
  Serial.println(highVal);
  Serial.print("Threshold (50%):      ");
  Serial.println(threshold);
  Serial.print("Final prox:           ");
  Serial.println(finalProx);
  Serial.print("Above threshold:      ");
  Serial.println((finalProx >= threshold) ? "YES" : "NO");
  Serial.print("Interrupt fired:      ");
  Serial.println(intStatus ? "YES" : "NO");

  Serial.println();

  // Determine pass/fail
  if (intStatus && (finalProx >= threshold)) {
    Serial.println("PASS: Servo triggered proximity interrupt successfully!");
  } else if (!intStatus && (finalProx >= threshold)) {
    Serial.println("FAIL: Prox above threshold but interrupt did not fire");
  } else if (intStatus && (finalProx < threshold)) {
    Serial.println(
        "FAIL: Interrupt fired but prox below threshold (spurious?)");
  } else {
    Serial.println("FAIL: Neither interrupt nor threshold crossing detected");
  }

  Serial.println();
  Serial.println("--- TEST COMPLETE ---");
}

void loop() {
  // Test runs once in setup
  delay(1000);
}
