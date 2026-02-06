/*!
 * @file 18_servo_prox_test.ino
 * @brief Automated proximity test using servo to trigger detection
 * @note Connect INT pin to D2, Servo to A0
 *       Servo 105° = paper in front of sensor
 *       Servo 0° = paper away from sensor
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
 * @param angle 105 = paper in front of sensor, 0 = paper away
 */
void moveServo(int angle) {
  testServo.write(angle);
  delay(500); // Allow servo to reach position
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Automated Servo Proximity Test");
  Serial.println("========================================");
  Serial.println("Hardware: INT=D2, Servo=A0");
  Serial.println("Servo 105deg = paper in front, 0deg = away");
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

  // Full reset and clean init
  apds.reset();
  delay(200);
  apds.begin();

  // Disable light sensor, enable prox only
  apds.enableLightSensor(false);
  apds.enableProximitySensor(true);

  // Good defaults for reliable readings
  apds.setLEDPulses(32);
  apds.setLEDCurrent(APDS9999_LED_CURRENT_25MA);

  // Set thresholds: low=10, high=20
  apds.setProxThresholdLow(10);
  apds.setProxThresholdHigh(20);

  // Set persistence to 1 for faster response
  apds.setProxPersistence(1);

  // Enable proximity interrupt
  apds.enableProxInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending interrupt
  apds.getMainStatus();
  intFired = false;

  Serial.println("Prox thresholds: Low=10, High=20");
  Serial.println("Interrupt enabled on D2 (FALLING)");
  Serial.println();

  // Begin test sequence
  Serial.println("--- TEST SEQUENCE START ---");
  Serial.println();

  // Step 1: Baseline reading with paper away
  Serial.println("Step 1: Servo at 0deg (paper away)");
  moveServo(0);
  delay(1000);

  uint16_t baselineProx;
  apds.readProximity(&baselineProx);
  Serial.print("  Baseline proximity: ");
  Serial.println(baselineProx);

  // Clear interrupt flag before triggering
  apds.getMainStatus();
  intFired = false;

  // Step 2: Move paper in front
  Serial.println();
  Serial.println("Step 2: Servo at 105deg (paper in front)");
  moveServo(105);
  delay(1000);

  uint16_t triggeredProx;
  apds.readProximity(&triggeredProx);
  Serial.print("  Triggered proximity: ");
  Serial.println(triggeredProx);

  // Check interrupt status
  uint8_t status = apds.getMainStatus();
  bool intStatus = intFired;
  Serial.print("  Interrupt fired: ");
  Serial.println(intStatus ? "YES" : "NO");
  Serial.print("  Status register: 0x");
  Serial.println(status, HEX);

  // Step 3: Move paper away
  Serial.println();
  Serial.println("Step 3: Servo back to 0deg (paper away)");
  moveServo(0);
  delay(500);

  // Final result
  Serial.println();
  Serial.println("--- TEST RESULTS ---");
  Serial.print("Baseline (away):   ");
  Serial.println(baselineProx);
  Serial.print("Triggered (front): ");
  Serial.println(triggeredProx);
  Serial.print("Prox increased:    ");
  Serial.println((triggeredProx > baselineProx) ? "YES" : "NO");
  Serial.print("Interrupt fired:   ");
  Serial.println(intStatus ? "YES" : "NO");
  Serial.print("Above threshold:   ");
  Serial.println((triggeredProx > 20) ? "YES" : "NO");

  Serial.println();

  // Determine pass/fail
  bool proxIncreased = (triggeredProx > baselineProx);
  bool aboveThreshold = (triggeredProx > 20);

  if (intStatus && proxIncreased && aboveThreshold) {
    Serial.println("PASS: Servo triggered proximity interrupt successfully!");
  } else if (proxIncreased && aboveThreshold && !intStatus) {
    Serial.println("PARTIAL: Prox detected but interrupt did not fire");
  } else if (!proxIncreased) {
    Serial.println("FAIL: Proximity did not increase with paper in front");
  } else {
    Serial.println("FAIL: Test conditions not met");
  }

  Serial.println();
  Serial.println("--- TEST COMPLETE ---");
}

void loop() {
  // Test runs once in setup
  delay(1000);
}
