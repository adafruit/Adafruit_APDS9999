/*!
 * @file 08_prox_interrupt_test.ino
 * @brief Automated proximity interrupt test with dynamic threshold calibration
 * @note Connect INT pin to D2, Servo to A0
 *       Servo 0° = paper away, 105° = paper in front of sensor
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

#define INT_PIN 2
#define SERVO_PIN A0
#define SERVO_AWAY 0
#define SERVO_CLOSE 105
#define SERVO_SETTLE_MS 500
#define INT_TIMEOUT_MS 3000
#define CALIBRATION_SAMPLES 5

Adafruit_APDS9999 apds;
Servo testServo;

volatile bool intFired = false;

void isr() { intFired = true; }

bool waitForInterrupt(uint16_t timeoutMs) {
  uint32_t start = millis();
  while (!intFired && (millis() - start < timeoutMs)) {
    delay(10);
  }
  return intFired;
}

uint16_t readProxAverage(uint8_t samples) {
  uint32_t sum = 0;
  uint16_t prox;
  for (uint8_t i = 0; i < samples; i++) {
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

  Serial.println(F("========================================"));
  Serial.println(F("APDS9999 Proximity Interrupt Test"));
  Serial.println(F("Self-Calibrating with Servo"));
  Serial.println(F("========================================"));

  bool allPassed = true;
  uint16_t prox;
  uint8_t status;

  // Initialize servo
  testServo.attach(SERVO_PIN);
  testServo.write(SERVO_AWAY);
  delay(SERVO_SETTLE_MS);
  Serial.println(F("Servo initialized at 0 deg (paper away)"));

  // Initialize sensor
  if (!apds.begin()) {
    Serial.println(F("FAIL: Could not find APDS9999 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("APDS9999 found!"));

  // Enable proximity sensor
  apds.enableProximitySensor(true);
  delay(50);

  // =============================================
  // PHASE 1: CALIBRATION
  // =============================================
  Serial.println(F("\n--- PHASE 1: Calibration ---"));

  // Step 1: Read baseline LOW value (paper away at 0°)
  Serial.println(F("Reading baseline (paper away)..."));
  uint16_t baseline = readProxAverage(CALIBRATION_SAMPLES);
  Serial.print(F("Baseline LOW: "));
  Serial.println(baseline);

  // Step 2: Move to 105° and read HIGH value
  Serial.println(F("Moving servo to 105 deg (paper close)..."));
  testServo.write(SERVO_CLOSE);
  delay(SERVO_SETTLE_MS);

  Serial.println(F("Reading high value (paper close)..."));
  uint16_t highVal = readProxAverage(CALIBRATION_SAMPLES);
  Serial.print(F("High value: "));
  Serial.println(highVal);

  // Step 3: Calculate thresholds
  uint16_t range = highVal - baseline;
  uint16_t lowThreshold = baseline + (range / 4);       // 25% up from baseline
  uint16_t highThreshold = baseline + ((range * 3) / 4); // 75% up from baseline

  Serial.println(F("\nCalculated thresholds:"));
  Serial.print(F("  Low threshold:  "));
  Serial.print(lowThreshold);
  Serial.println(F(" (25% of range)"));
  Serial.print(F("  High threshold: "));
  Serial.print(highThreshold);
  Serial.println(F(" (75% of range)"));

  // Sanity check
  if (range < 50) {
    Serial.println(F("WARNING: Range too small, calibration may be unreliable!"));
  }

  // =============================================
  // PHASE 2: INTERRUPT TEST
  // =============================================
  Serial.println(F("\n--- PHASE 2: Interrupt Test ---"));

  // Servo is already at 105° (close) from calibration

  // Set calculated thresholds
  apds.setProxThresholdLow(lowThreshold);
  apds.setProxThresholdHigh(highThreshold);
  Serial.print(F("Thresholds applied: Low="));
  Serial.print(lowThreshold);
  Serial.print(F(", High="));
  Serial.println(highThreshold);

  // Set persistence
  apds.setProxPersistence(4);
  Serial.println(F("Persistence set to 4"));

  // Enable proximity interrupt
  apds.enableProxInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending interrupt
  apds.getMainStatus();
  intFired = false;

  // --- TEST 1: Low threshold interrupt (move paper away) ---
  Serial.println(F("\n--- TEST 1: Low threshold interrupt ---"));
  Serial.println(F("Moving servo to 0 deg (paper away)..."));
  testServo.write(SERVO_AWAY);
  delay(SERVO_SETTLE_MS);

  // Wait for LOW interrupt
  if (waitForInterrupt(INT_TIMEOUT_MS)) {
    Serial.println(F("Interrupt fired!"));

    // Read status to clear interrupt
    status = apds.getMainStatus();
    Serial.print(F("Status: 0x"));
    Serial.println(status, HEX);

    // Read proximity
    apds.readProximity(&prox);
    Serial.print(F("Proximity reading: "));
    Serial.println(prox);

    if (prox < lowThreshold) {
      Serial.println(F("TEST 1 PASS: Prox below low threshold"));
    } else {
      Serial.println(F("TEST 1 FAIL: Prox not below low threshold"));
      allPassed = false;
    }
  } else {
    Serial.println(F("TEST 1 FAIL: Interrupt timeout!"));
    apds.readProximity(&prox);
    Serial.print(F("Current proximity: "));
    Serial.println(prox);
    allPassed = false;
  }

  // Clear interrupt state
  apds.getMainStatus();
  intFired = false;

  // --- TEST 2: High threshold interrupt (move paper close) ---
  Serial.println(F("\n--- TEST 2: High threshold interrupt ---"));
  Serial.println(F("Moving servo to 105 deg (paper close)..."));
  testServo.write(SERVO_CLOSE);
  delay(SERVO_SETTLE_MS);

  // Wait for HIGH interrupt
  if (waitForInterrupt(INT_TIMEOUT_MS)) {
    Serial.println(F("Interrupt fired!"));

    // Read status to clear interrupt
    status = apds.getMainStatus();
    Serial.print(F("Status: 0x"));
    Serial.println(status, HEX);

    // Read proximity
    apds.readProximity(&prox);
    Serial.print(F("Proximity reading: "));
    Serial.println(prox);

    if (prox > highThreshold) {
      Serial.println(F("TEST 2 PASS: Prox above high threshold"));
    } else {
      Serial.println(F("TEST 2 FAIL: Prox not above high threshold"));
      allPassed = false;
    }
  } else {
    Serial.println(F("TEST 2 FAIL: Interrupt timeout!"));
    apds.readProximity(&prox);
    Serial.print(F("Current proximity: "));
    Serial.println(prox);
    allPassed = false;
  }

  // Final summary
  Serial.println(F("\n========================================"));
  if (allPassed) {
    Serial.println(F("RESULT: ALL TESTS PASSED"));
  } else {
    Serial.println(F("RESULT: SOME TESTS FAILED"));
  }
  Serial.println(F("========================================"));

  // Detach servo to stop jitter
  testServo.detach();
}

void loop() {
  // Test complete - nothing to do
  delay(1000);
}
